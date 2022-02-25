/*
 *  Copyright (c) 2020 Henrik Granö
 *  See the license in the root directory for the full notice
 *
 *  Class for routing, relaxation, Python API interfacing and some directory setting.
*/

#include "vhelix.h"


// Python.h includes "slots" as well, resulting in a conflict. This fixes the issue.
#pragma push_macro("slots")
#undef slots
#define MS_NO_COREDLL
#ifdef _DEBUG
    #undef _DEBUG
    #include "Python.h"
    #define _DEBUG
#else
    #include "Python.h"
#endif
#pragma pop_macro("slots")

std::string vHelixFunc::ExePath() {
    char buffer[MAX_PATH];
    GetModuleFileName( NULL, buffer, MAX_PATH );
    std::string::size_type pos = std::string( buffer ).find_last_of( "\\/" );
    return std::string( buffer ).substr( 0, pos);
}

vHelix::vHelix() : meshdir_(""), mesh_(""), workspace_dir_(""), plydata_(nullptr), design(nullptr)
{
    //Initialize Python interpreter
    Py_Initialize();
    std::cerr << "Python initialized"<< std::endl;
    // Set workspace directory
    std::stringstream path(vHelixFunc::ExePath());
    std::string segment;
    std::vector<std::string> seglist;
    while (std::getline(path, segment, '\\')) {
        seglist.push_back(segment);
    }
    seglist.pop_back();
    // seglist.pop_back();  // not used in public release
    seglist.push_back(std::string("workspace"));
    workspace_dir_ = std::accumulate(std::begin(seglist), std::end(seglist), std::string(),
                             [](std::string &ss, std::string &s)
                             {
                                 return ss.empty() ? s : ss + "\\" + s;
                             });
}

vHelix::~vHelix()
{
    if (plydata_) {
        delete plydata_;
    }
    delete design;
    finished_();
    std::cerr << "Python finalized"<< std::endl;
    Py_Finalize();
}

// Read meshfile and copy it to the workspace directory. Because Bscor-modules
// use relative paths (therefore making it impossible to directly use files on potentially other
// drives) it is easier to copy the file.
void vHelix::readMesh_(QString qmesh)
{
    std::string mesh = qmesh.toStdString();
    meshdir_ = mesh;
    for (unsigned int i = 0; i < meshdir_.length(); i++) {
        if (meshdir_[i] == '/') {
            meshdir_[i] = '\\';
        }
    }
    std::stringstream filename(mesh);
    std::string segment;
    std::vector<std::string> seglist;
    while (std::getline(filename, segment, '/')) {
        seglist.push_back(segment);
    }
    mesh_ = seglist.back();
    /*if (mesh_.find(".obj") != std::string::npos) {
        convert_("obj_to_ply");
        return;
    }*/
    // copy the meshfile to workspace directory
    std::stringstream workspace_file;
    workspace_file << workspace_dir_ << "\\" << mesh_;
    std::string temp(workspace_file.str());
    if (QFile::copy(meshdir_.c_str(), temp.c_str())) {
        // success
    }
}

void vHelix::setSelection(std::vector<std::string> vec)
{
    fileSelection_ = vec;
}

// used in openPLY
bool vHelixFunc::compareEdges(std::vector<int> i, std::vector<int> j)
{
    return i[0] < j[0];
}

// Open ascii .ply file for file info (used to estimate base use in the physical relaxation menu)
void vHelix::openPLY() {
    std::stringstream sstr;
    sstr << vHelixFunc::ExePath() << "/../workspace/" << mesh_;
    std::string input_ply_file = sstr.str();
    std::ifstream inply(input_ply_file);
    int vertexnr, vertices, facenr, faces, content;
    content = 0;
    vertices = 0;
    faces = 0;
    std::vector<float> x, y, z;
    std::vector<std::pair<float, float>> doublevec;
    std::vector<std::vector<int>> facevec;
    std::string line;
    while (std::getline(inply, line)) {
        std::string segment;
        std::vector<std::string> seglist;
        std::stringstream segsstr(line);
        while (std::getline(segsstr, segment, ' ')) {
            seglist.push_back(segment);
        }
        if (line.find("end_header") != std::string::npos) {
            content = 1;
            continue;
        }
        else if (line.find("vertex ") != std::string::npos) {
            try {
                vertexnr = std::stoi(seglist[2]);
                continue;
            } catch (const std::bad_cast &e) {
                sendToConsole_("POP_ERRBad header in .ply");
                return;
            }
        }
        else if (line.find("face ") != std::string::npos) {
            try {
                facenr = std::stoi(seglist[2]);
                continue;
            } catch (const std::bad_cast &e) {
                sendToConsole_("POP_ERRBad header in .ply");
                return;
            }
        }
        else if (line.find("comment double") != std::string::npos) {
            try {
                doublevec.push_back(std::make_pair(std::stof(seglist[2]), std::stof(seglist[3])));
            } catch (const std::bad_cast &e) {
                sendToConsole_("POPUP_ERRBad vector coordinates (failed cast to float) in .ply\nPOPUP_END");
                return;
            }
            continue;
        }
        if (content) {
            if (vertices < vertexnr) {
                try {
                    x.push_back(std::stof(seglist[0]));
                    y.push_back(std::stof(seglist[1]));
                    z.push_back(std::stof(seglist[2]));
                } catch (const std::bad_cast &e) {
                    sendToConsole_("POPUP_ERRBad vertex coordinates (failed cast to float) in .ply\nPOPUP_END");
                }
                vertices++;
                continue;
            }
            else {
                if (faces < facenr) {
                    try {
                        std::vector<int> vec;
                        for (unsigned int i = 1; i < seglist.size(); i++) {
                            vec.push_back(std::stoi(seglist[i]));
                        }
                        facevec.push_back(vec);
                    } catch (const std::bad_cast &e) {
                        sendToConsole_("POPUP_ERRBad face vertex data (failed cast to int) in .plyPOPUP_END");
                    }
                    faces++;
                    continue;
                }
            }
        }
    }
    std::vector<std::vector<int>> temp;
    std::vector<int> tempvec;
    for (auto k : facevec) {
        k.push_back(k[0]);
        for (unsigned int j = 0; j < k.size() -1; j++) {
            tempvec.push_back(k[j]);
            tempvec.push_back(k[j+1]);
            std::sort(tempvec.begin(), tempvec.end());
            temp.push_back(tempvec);
            tempvec.clear();
        }
    }
    std::sort(temp.begin(), temp.end(), vHelixFunc::compareEdges);
    std::vector<std::vector<int>> edgelist;
    bool duplicate = false;
    for (auto edge : temp) {
        for (auto doubleEdge : doublevec) {
            if (edge[0] == doubleEdge.first && edge[1] == doubleEdge.second) {
                duplicate = true;
            }
        }
        if (!duplicate) {
            edgelist.push_back(edge);
        }
        duplicate = false;
    }

    plydata_ = new vHelixFunc::plyData;
    plydata_->x = x;
    plydata_->y = y;
    plydata_->z = z;
    plydata_->edgelist = edgelist;
    plydata_->vertexnr = vertexnr;
    plydata_->facenr = facenr;
}

void vHelix::open3d() {
    
    std::stringstream sstr;
    std::string dir("../workspace/");
    sstr << dir << mesh_;
    std::string name(sstr.str());
    name.resize(name.size() - 4);
    Design des(sstr.str(),name);
    des.read3Dobject();
}

const vHelixFunc::plyData vHelix::getPlyData()
{
    return *plydata_;
}

// Receive commands and data from MainWindow. QVariant is a convenient template data type for this.
void vHelix::action_(QString qcmd, QVector<QVariant> arg)
{
    std::string cmd = qcmd.toStdString();
    if (cmd.find("Atrail") != std::string::npos) {
        if (mesh_ == "") {
            sendToConsole_(QString("No mesh loaded.\n"));
        }
        std::stringstream msg;
        msg << "Running Atrail routing on " << mesh_ << "..." << std::endl;
        sendToConsole_(QString(msg.str().c_str()));
        vHelix *tmp = this;
        QFutureWatcher<void> watcher;
        //QFuture<void> f = QtConcurrent::run(tmp,&vHelix::atrail_,arg); //run in different thread to keep main window responsive
        atrail_(arg);
        
    }
    else if (cmd.find("Scaffold free") != std::string::npos) {
        if (mesh_ == "") {
            sendToConsole_(QString("No mesh loaded.\n"));
        }
        std::stringstream msg;
        msg << "Running scaffold-free design on " << mesh_ << "..." << std::endl;
        std::string str = msg.str();
        sendToConsole_(QString(str.c_str()));
        QtConcurrent::run(this,&vHelix::scaffold_free_,arg);
    }
    else if (cmd.find("Spanning tree") != std::string::npos) {
        sendToConsole_(QString("Spanning tree routing is not yet implemented"));
    }
    else if (cmd.find("setselection") != std::string::npos) {
        std::vector<std::string> strvec;
        for (auto qstr : arg) {
            QString qs;
            std::string s;
            qs = qstr.toString();
            if (_WINDOWS) {
                s = qs.toLocal8Bit().constData();
            }
            else {
                s = qs.toUtf8().constData();
            }
            if (s.find(".ply") != std::string::npos || s.find(".obj") != std::string::npos) {
                std::stringstream path(s);
                std::string segment;
                std::vector<std::string> seglist;
                while (std::getline(path, segment, '/')) {
                    seglist.push_back(segment);
                }
                mesh_ = seglist.back();
            }
            strvec.push_back(s);
        }
        setSelection(strvec);
    }
    else if (cmd.find("PhysX") != std::string::npos) {
        std::stringstream msg;
        msg << "Running PhysX strand relaxation on " << mesh_ << std::endl << std::endl;
        std::string str = msg.str();
        sendToConsole_(QString(str.c_str()));
        //physX_relaxation_(arg);
    }
    else if (cmd.find("export") != std::string::npos) {
        std::stringstream msg;
        msg << "Exporting selected file... \n";
        // file selected from dialog in mainwindow.cpp
        // arg should be: arg[0]: file, [1]: target type
        export_(arg);
    }
    else if (cmd.find("estimate bases") != std::string::npos) {
        std::pair<int,int> estimate = estimate_base_use_(arg);
        std::stringstream retsstr;
        if (arg[1].toString().toStdString().find("Scaffold free")) {
             retsstr << "Estimated base use: " << std::get<0>(estimate) << " + " << std::get<1>(estimate) << " (filled strand gaps)\n";
        }
        else {
            retsstr << "Estimated scaffold strand length: " << std::get<0>(estimate) << " + " << std::get<1>(estimate) << " (filled strand gaps)\n";
        }
        
        std::string ret = retsstr.str();
        sendToConsole_(QString(ret.c_str()));
    }
    else if (cmd.find("oxdna_view") != std::string::npos) {
        unsigned char top = 255; unsigned char conf = 255;
        for (unsigned char i = 0; i < fileSelection_.size(); i++) {
            if (fileSelection_.at(i).find(".top") != std::string::npos) {
                top = i;
            }
            if (fileSelection_.at(i).find(".oxdna") != std::string::npos) {
                conf = i;
            }
        }
        /*if (top == 255 || conf == 255) {
            sendToConsole_("No valid configuration (.oxdna) and topology (.top) files selected! Select both and run the oxdna viewer command.");
            return;
        }*/

        #ifdef __linux__
                // Linux default browser
                std::stringstream path;
                path << "open " << vHelixFunc::ExePath() << "/../vHelix/oxdnaviewer/index.html";
                std::string browser(path.str());
                system("open " + browser.c_str());
        #elif _WIN32
        #include <Windows.h>
        #include <shellapi.h>
        std::stringstream path;
        path << vHelixFunc::ExePath() << "/../vHelix/oxdnaviewer/index.html";
        std::string index_html = path.str();
        /*path << "?" <<
                "configuration=" << fileSelection_.at(conf) << "&" <<
                "topology=" << fileSelection_.at(top);*/
        std::string browser(path.str());
        browser.insert(0, "file:///");
        sendToConsole_(QString(browser.c_str()));

        int result = 0;
        TCHAR app[MAX_PATH] = {0};
        result = (int)::FindExecutable(_T(index_html.c_str()), NULL, app);
        if (result > 32) {
            ::ShellExecute(0, NULL, app, _T(browser.c_str()), NULL, SW_SHOWNORMAL);
        }

        //ShellExecute(0, 0, browser.c_str(), 0, 0, SW_SHOW);
        #endif
    }
}

void vHelix::atrail_(const QVector<QVariant> &args) {
    if (design != nullptr) {
        delete design;
    }
    std::cerr << "In atrail_ function\n";
    if (fileSelection_.size() != 1 ||
            (fileSelection_.at(0).find(".ply") == std::string::npos && fileSelection_.at(0).find(".obj") == std::string::npos)) {
        sendToConsole_(QString("POPUP_ERRSelect a single valid .ply mesh file!\nPOPUP_END"));
        return;
    }
    std::stringstream sstr;
    std::string dir("../workspace/");
    sstr << dir << mesh_;
    std::string name(sstr.str());
    name.resize(name.size() - 4);
    std::string rpoly(name);
    rpoly.append(".rpoly");
    std::cerr << name.c_str() << "\n" << sstr.str().c_str() << "\n";
    design = new Atrail(sstr.str(),name);
    std::cerr << "Created atrail object\n";
    int ret = design->main();
    sendToConsole_(design->getoutstream().c_str());
    if (ret != 1) {
        return;
    }
    //std::thread relaxthread(design->relax,QVector<QVariant>({args[0]}));
    //ret = design->relax(QVector<QVariant>({args[0]}));
    bool hasresult = false;
    //std::thread relaxthread(&Atrail::relax,design,std::ref(QVector<QVariant>({args[0]})),std::ref(hasresult));
    //relaxthread.join();
    ret = design->relax(QVector<QVariant>({args[0]}),hasresult);
    sendToConsole_(design->getoutstream().c_str());
    if (ret != 1) {
        return;
    }
    //design->model_.readRpoly(rpoly.c_str(),args[1].toInt(),args[2].toInt());
    design->model_.readRpoly2(design->getRpoly(),args[1].toInt(),args[2].toInt());
    if (args[3].toBool()) {
        sendToConsole_("Autofilling strand gaps\n");
        design->model_.autofillStrandGaps();
    }
    std::string seq = args[4].toString().toStdString();
    std::cout << "Length of sequenceVHELIX: " << seq.length() << "\n"; 
    if (seq.length() > 0) {
        
        design->generate_sequences(seq);
    }
    design->writeOxView();
    sendToConsole_(design->getoutstream().c_str());
}

/*void vHelix::atrail_(const QVector<QVariant> &args) {
    if (design != nullptr) {
        delete design;
    }
    std::cerr << "In atrail_ function\n";
    if (fileSelection_.size() != 1 ||
            (fileSelection_.at(0).find(".ply") == std::string::npos && fileSelection_.at(0).find(".obj") == std::string::npos)) {
        sendToConsole_("POPUP_ERRSelect a single valid .ply mesh file!\nPOPUP_END");
        return;
    }
    std::stringstream sstr;
    std::string dir("../workspace/");
    sstr << dir << mesh_;
    std::string name(sstr.str());
    name.resize(name.size() - 4);
    std::string rpoly(name);
    rpoly.append(".rpoly");
    std::cerr << name.c_str() << "\n" << sstr.str().c_str() << "\n";
    Atrail des(sstr.str(),name);
    std::cerr << "Created atrail object\n";
    int ret = des.main();
    sendToConsole_(des.getoutstream());
    if (ret != 1) {
        return;
    }
    //std::thread relaxthread(design->relax,QVector<QVariant>({args[0]}));
    //ret = design->relax(QVector<QVariant>({args[0]}));
    bool hasresult = false;
    QVector<QVariant> qarg({args[0]});

    std::thread relaxthread(&Atrail::relax,&des,qarg.begin(),std::ref(hasresult));
    relaxthread.join();
    //ret = design->relax(QVector<QVariant>({args[0]}),hasresult);
    sendToConsole_(des.getoutstream());
    if (!hasresult) {
        return;
    }
    //design->model_.readRpoly(rpoly.c_str(),args[1].toInt(),args[2].toInt());
    des.model_.readRpoly2(design->getRpoly(),args[1].toInt(),args[2].toInt());
    if (args[3].toBool()) {
        sendToConsole_("Autofilling strand gaps\n");
        des.model_.autofillStrandGaps();
    }
    std::string seq = args[4].toString().toStdString();
    std::cout << "Length of sequenceVHELIX: " << seq.length() << "\n"; 
    if (seq.length() > 0) {
        
        des.generate_sequences(seq);
    }
    des.writeOxView();
    sendToConsole_(des.getoutstream());
}*/

void vHelix::scaffold_free_(const QVector<QVariant> &args) {
    std::cerr << "In scaffold_free_ function\n";
    if (fileSelection_.size() != 1 ||
            (fileSelection_.at(0).find(".ply") == std::string::npos && fileSelection_.at(0).find(".obj") == std::string::npos)) {
        sendToConsole_("POPUP_ERRSelect a single valid .ply/.obj mesh file!\nPOPUP_END");
        return;
    }
    std::stringstream sstr;
    std::string dir("../workspace/");
    sstr << dir << mesh_;
    std::string name(sstr.str());
    name.resize(name.size() - 4);
    std::string rpoly(name);
    rpoly.append(".rpoly");
    std::cerr << name.c_str() << "\n" << sstr.str().c_str() << "\n";
    design = new Scaffold_free(sstr.str(),name);
    //Atrail atrail(sstr.str(),name);
    std::cerr << "Created scaffoldfree object\n";
    design->main();
    sendToConsole_(design->getoutstream().c_str());
    bool hasresult = false;
    int ret = design->relax(QVector<QVariant>({args[0]}),hasresult);
    sendToConsole_(design->getoutstream().c_str());
    std::cout << "Reading rpoly\n";
    //design->model_.readScaffoldFreeRpoly(rpoly.c_str());
    design->model_.readScaffoldFreeRpoly2(design->getRpoly());
    if (args[1].toBool()) {
        sendToConsole_("Autofilling strand gaps\n");
        design->model_.autofillStrandGaps();
    }
    design->generate_sequences(args[2].toString().toStdString());
    design->writeOxView();
    sendToConsole_(design->getoutstream().c_str());
}
/*
void vHelix::atrail_()
{
    // Run ply_to_dimacs
    /*
    *  Takes the graph information of a 3D object's mesh (ply file) and creates a graph (dimacs file).
    *  Argument 1: filename of ply document (ply ascii format http://paulbourke.net/dataformats/ply).
    *  Argument 2 (Optional): output graph filename in dimacs format, uses the ply file basename + dimacs if not given (dimacs format mat.gsia.cmu.edu/COLOR/general/ccformat.ps).

    if (fileSelection_.size() != 1 ||
            fileSelection_.at(0).find(".ply") == std::string::npos) {
        sendToConsole_("POPUP_ERRSelect a single valid .ply mesh file!\n");
    }
    sendToConsole_("\n\nRunning ply_to_dimacs\n");
    std::stringstream sstr;
    std::string dir("../workspace/");
    sstr << dir << mesh_;
    std::string name("ply_to_dimacs");
    std::string temp(sstr.str());
    const char* argv[] = {name.c_str(), temp.c_str()};
    int argc = sizeof(argv) / sizeof(char*) - 1;
    ply_to_dimacs::main(argc, argv, *this);

    // Run ply_to_embedding
    /*
    *  Takes the embedding information of a 3D object's mesh (ply file) and creates an embedding (vcode file).
    *  Argument 1: filename of ply document (ply ascii format http://paulbourke.net/dataformats/ply).
    *  Argument 2 (Optional): output embedding filename in vcode format, uses the ply file basename + vcode if not given (vcode is simply list of adjacent vertices according to their clockwise order).
    *  Preconditions: TODO: mesh is 2-vertex-connected

    sendToConsole_("\n\nRunning ply_to_embedding\n");
    sstr.str(std::string());
    sstr << dir << mesh_;
    name.clear();
    name = "ply_to_embedding";
    temp.clear();
    temp = sstr.str();
    const char* argv2[] = {name.c_str(), temp.c_str()};
    ply_to_embedding::main(argc, argv2, *this);

    // Run postman_tour
    /*
    *  Creates a multigraph from a simple graph such that the graph is Eulerian
    *  Argument 1: filename of input graph in dimacs format.
    *  Argument 2: filename of output multigraph in dimacs format.

    sendToConsole_("\n\nRunning postman_tour\n");
    sstr.str(std::string());
    std::string dimacs(mesh_);
    std::string temp2;
    dimacs.resize(dimacs.size() - 4);
    std::string multigraph_dimacs(dimacs);
    dimacs.append(".dimacs");
    multigraph_dimacs.append("_multi.dimacs");
    sstr << dir << dimacs;
    name.clear();
    name = "postman_tour";
    temp.clear();
    temp = sstr.str();
    sstr.str(std::string());
    sstr << dir << multigraph_dimacs;
    temp2 = sstr.str();
    const char* argv3[] = {name.c_str(), temp.c_str(), temp2.c_str()};
    postman_tour::main(3, argv3, *this);

    /*
    *  Creates an eulerian embedding as an edge code from an embedding given as an vertex code and the associated multigraph containing the multiedges.
    *  Argument 1: filename of input embedding in vcode format.
    *  Argument 2: filename of input multigraph in dimacs format.
    *  Argument 3 (optional): filename of output edge code, if not given the input vcode filename with an extension .ecode is used.
    *
    sendToConsole_("\n\nRunning Make_embedding_eulerian\n");
    sstr.str(std::string());
    dimacs.resize(dimacs.size() - 6);
    dimacs.append("vcode");
    sstr << dir << dimacs;
    temp = sstr.str();
    name.clear();
    name = "make_embedding_eulerian";
    sstr.str(std::string());
    sstr << dir << multigraph_dimacs;
    temp2 = sstr.str();
    const char* argv4[] = {name.c_str(), temp.c_str(), temp2.c_str()};
    for (auto k : argv4) {
        std::cout << k << " -- ";
    }
    std::cout << std::endl;
    make_embedding_eulerian::main(3, argv4, *this);

    /*
    * Searches for an A-trail from an edge code file.
    * Argument 1: the edge code file.
    * Argument 2 (Optional): a trail file as a sequence of edge indices. This has an extension '.trail'.
    * Argument 3 (Optional): a trail file as a sequence of node indices(zero based). The trail ends with the vertice it began. The output file has extension `.ntrail'.
    *
    sendToConsole_("\n\nRunning Atrail_search\n");
    sstr.str(std::string());
    sstr << dir << mesh_;
    name.clear();
    name = "Atrail_search";
    temp.clear();
    temp = sstr.str();
    temp.resize(temp.size()-3);
    temp.append("ecode");
    const char* argv5[] = {name.c_str(), temp.c_str()};
    atrail_search::main(2, argv5, *this);

    /*
    *  Verifies that an edge trail generated by an Atrail_search is in fact an A-trail with respect to the embedding given as an edge code.
    *  Argument 1: the edge code file.
    *  Argument 2: the edge trail file.
    *
    sendToConsole_("\n\nRunning Atrail_verify\n");
    sstr.str(std::string());
    std::string trail(temp);
    trail.resize(trail.size() - 5);
    trail.append("trail");
    name.clear();
    name = "Atrail_verify";
    const char* argv6[] = {name.c_str(), temp.c_str(), trail.c_str()};
    atrail_verify::main(3, argv6, *this);
}
*/

// convert rpoly to oxdna
void vHelix::export_(const QVector<QVariant> &args)
{
    // Python scripts run through the C++/Python API
    std::stringstream cmd;
    if (fileSelection_.size() == 0) {
        sendToConsole_("No file chosen!\n");
        return;
    }
    else if (fileSelection_.size() > 1) {
        sendToConsole_("Select only a single file!\n");
        return;
    }
    QString func = args.at(0).toString();
    std::string funcStr;
    if (_WINDOWS) {
        funcStr = func.toLocal8Bit().constData();
    }
    else {
        funcStr = func.toUtf8().constData();
    }
    std::string path = vHelixFunc::ExePath();

    if (funcStr == "rpoly_oxDNA") {
        std::string file = fileSelection_[0];
        if (!file.find(".rpoly")) {
            sendToConsole_("Only .rpoly files supported for OxDNA conversion!\n");
            return;
        }

        std::stringstream split(file);
        std::string segment;
        std::vector<std::string> seglist;
        while(std::getline(split, segment, '/'))
        {
           seglist.push_back(segment);
        }
        std::string filename = seglist.back();

        sendToConsole_("Converting .PLY to .oxDNA...\n");
        //Py_Initialize();

        std::cerr << "In the event handler\n";

        PyObject *pName = NULL;
        pName = PyUnicode_FromString("rpoly_oxDNA");

        std::cerr << "Before module import: " <<
                    "\n pName:" << Py_REFCNT(pName) << "\n";

        PyObject *pModule = NULL;
        pModule = PyImport_Import(pName);
        std::cerr << "After module import: "
                    "\n pModule:" << Py_REFCNT(pModule) <<
                    "\n pName:" << Py_REFCNT(pName) << "\n";
        if (pModule == NULL) {
            sendToConsole_("--- Something went wrong when importing rpoly_oxDNA.py ---\n");
            PyErr_PrintEx(1);
            Py_DECREF(pName);
            return;
        }
        std::cerr <<"Module imported successfully\n";
        PyObject *pFunc = PyObject_GetAttrString(pModule, "main");

        if (pFunc && PyCallable_Check(pFunc)) {
            std::cerr << "Callable function recognized\n";
            PyObject *pArgs = PyTuple_New(1);
            std::stringstream sstr;
            sstr << file;
            std::string input_file = sstr.str();
            std::cerr << "BEFORE pValue set call: "
                        "\n pModule:" << Py_REFCNT(pModule) <<
                        "\n pArgs:" << Py_REFCNT(pArgs) <<
                        "\n pName:" << Py_REFCNT(pName) <<
                        "\n pFunc:" << Py_REFCNT(pFunc) << "\n";
            PyObject *pValue = PyUnicode_FromString(input_file.c_str());
            std::cerr << "BEFORE args set call: "
                        "\n pModule:" << Py_REFCNT(pModule) <<
                        "\n pArgs:" << Py_REFCNT(pArgs) <<
                        "\n pValue:" << Py_REFCNT(pValue) <<
                        "\n pName:" << Py_REFCNT(pName) <<
                        "\n pFunc:" << Py_REFCNT(pFunc) << "\n";
            //Py_INCREF(pValue);
            PyTuple_SetItem(pArgs, 0, pValue); //pArgs steals the reference to pValue - pValue is a borrows reference

            std::cerr << "Function args set\n";

            std::cerr << "BEFORE func call: "
                        "\n pModule:" << Py_REFCNT(pModule) <<
                        "\n pArgs:" << Py_REFCNT(pArgs) <<
                        "\n pValue:" << Py_REFCNT(pValue) <<
                        "\n pName:" << Py_REFCNT(pName) <<
                        //"\n pRet:" << Py_REFCNT(pRet) <<
                        "\n pFunc:" << Py_REFCNT(pFunc) << "\n";
            
            PyObject *pRet = PyObject_CallObject(pFunc, pArgs);
            std::cerr << "Python function called successfully\n";
            std::cerr << "BEFORE DECREF: "
                        "\n pModule:" << Py_REFCNT(pModule) <<
                        "\n pArgs:" << Py_REFCNT(pArgs) <<
                        "\n pValue:" << Py_REFCNT(pValue) <<
                        "\n pName:" << Py_REFCNT(pName) <<
                        "\n pRet:" << Py_REFCNT(pRet) <<
                        "\n pFunc:" << Py_REFCNT(pFunc) << "\n";
            Py_DECREF(pFunc);
            Py_DECREF(pArgs);
            Py_DECREF(pRet);
            Py_DECREF(pModule);
            // Py_DECREF(pValue); pArgs stole the reference and frees it
            Py_DECREF(pName);

            std::cerr << "AFTER DECREF: "
                        "\n pModule:" << Py_REFCNT(pModule) <<
                        "\n pArgs:" << Py_REFCNT(pArgs) <<
                        "\n pValue:" << Py_REFCNT(pValue) <<
                        "\n pName:" << Py_REFCNT(pName) <<
                        "\n pRet:" << Py_REFCNT(pRet) <<
                        "\n pFunc:" << Py_REFCNT(pFunc) << "\n";



            //Py_DECREF(pModule);



            std::cerr << "Values dereferenced\n";
            // copy files from executable directory to workspace
            std::stringstream workspace_oxDNA_file, workspace_top_file;

            workspace_oxDNA_file << workspace_dir_ << "\\" << filename << ".oxdna";
            workspace_top_file << workspace_dir_ << "\\" << filename << ".top";
            std::string oxDNA_temp(workspace_oxDNA_file.str());
            std::string top_temp(workspace_top_file.str());
            //std::cerr << oxDNA_temp.c_str() << ": temp\n";
            std::stringstream oxDnadir, topdir;
            oxDnadir << path << "\\" << filename << ".oxdna";
            topdir << path << "\\" << filename << ".top";
            std::string oxDNA_copydir = oxDnadir.str();
            std::string top_copydir = topdir.str();
            std::cerr << oxDNA_copydir.c_str() << ": copydir\n";
            if (QFile::rename(oxDNA_copydir.c_str(), oxDNA_temp.c_str())
            &&  QFile::rename(top_copydir.c_str(), top_temp.c_str())) {
                sendToConsole_("Wrote .oxDNA and .top file to workspace\n");
            }
            else {
                sendToConsole_("Failed to write files to workspace. "
                "Delete the existing files with same file names and try again.\n");
            }
        }
        else {
            sendToConsole_("Conversion failed, check your Python version or .rpoly file");
            if (pFunc) {
                Py_DECREF(pName);
            }
            Py_DECREF(pFunc);
            Py_DECREF(pModule);
            //Py_Finalize();
            return;
        }
        //Py_Finalize();
    }
}


// convert obj to ply
void vHelix::convert_(const std::string& format)
{
    // Python scripts run through the C++/Python API

    std::stringstream cmd;
    std::string path = vHelixFunc::ExePath();
    if (mesh_ == "") {
        sendToConsole_("No mesh chosen!\n");
    }
    std::string file(mesh_);
    if (format == "obj_to_ply") {
        sendToConsole_("Converting .obj to .ply...\n");
        //Py_Initialize();
        PyObject *pName = PyUnicode_FromString("convert");
        PyObject *pModule = PyImport_Import(pName);
        if (pModule == NULL) {
            sendToConsole_("--- Something went wrong when importing convert.py ---\n");
            PyErr_PrintEx(1);
            //Py_Finalize();
            return;
        }
        PyObject *pFunc = PyObject_GetAttrString(pModule, "main");
        if (pFunc && PyCallable_Check(pFunc)) {
            PyObject *pArgs = PyTuple_New(2);
            std::stringstream sstr;
            PyErr_PrintEx(1);       // ERR
            sstr << meshdir_;
            std::string input_file = sstr.str();
            PyObject *pValue = PyUnicode_FromString(input_file.c_str());
            PyTuple_SetItem(pArgs, 0, pValue);
            sstr.str(std::string());
            std::string plyfile(file);
            plyfile.resize(plyfile.size() - 3);
            plyfile.append("ply");
            sstr << path << "/../workspace/" << plyfile;
            std::string output_file = sstr.str();
            PyObject *pValue2 = PyUnicode_FromString(output_file.c_str());
            PyTuple_SetItem(pArgs, 1, pValue2);
            PyObject_CallObject(pFunc, pArgs);
            PyErr_PrintEx(1);       // Error output to stderr, if any
            // copy files from executable directory to workspace
            std::stringstream workspace_file;
            workspace_file << workspace_dir_ << "\\" << file;
            std::string temp(workspace_file.str());
            std::stringstream objdir;
            objdir << path << "/" << file;
            std::string copydir = objdir.str();
            QFile::copy(copydir.c_str(), temp.c_str());
            objdir.str(std::string());
            objdir << path << "/" << plyfile;
            copydir = objdir.str();
            workspace_file.str(std::string());
            workspace_file << workspace_dir_ << "\\" << plyfile;
            temp = workspace_file.str();
            QFile::copy(copydir.c_str(), temp.c_str());
            sendToConsole_("Wrote .obj and .ply files to workspace\n");
        }
        else {
            sendToConsole_("Conversion failed, check your Python version or .obj file");
            //Py_Finalize();
            return;
        }
        PyErr_Print();
        PyErr_Clear();
        //Py_Finalize();
    }

}

// estimate base use for structure based on relaxation scaling value (/2 = scaffold strand length)
std::pair<int,int> vHelix::estimate_base_use_(const QVector<QVariant> &args)
{
    std::cout << "In estimate_base_use_\n";
    std::stringstream sstr;
    std::string dir("../workspace/");
    sstr << dir << mesh_;
    std::string name(sstr.str());
    name.resize(name.size() - 4);
    Design des(sstr.str(),name);
    des.read3Dobject();

    double scaling = args[1].toDouble();
    std::string type = args[0].toString().toStdString();
    //design = new Design();
    std::vector<std::vector<double>> vertices;
    des.get_coordinates(vertices);
    size_t vertexnr = vertices.size();
    std::cout  << vertices.size() << "\n";
    std::vector<std::vector<int>> elist;
    des.get_edgelist(elist);
    
    std::cout << elist.size() << "\n";
    double totalLength = 0.0;
    for (auto &edge : elist) {
        //totalLength = totalLength + std::sqrt(std::pow(plydata_->x[edge[0]] - plydata_->x[edge[1]], 2) +
        //        std::pow(plydata_->y[edge[0]] - plydata_->y[edge[1]], 2) +
        //        std::pow(plydata_->z[edge[0]] - plydata_->z[edge[1]], 2) );
        totalLength = totalLength + std::sqrt(std::pow(vertices[edge[0]][0] - vertices[edge[1]][0], 2) +
                std::pow(vertices[edge[0]][1] - vertices[edge[1]][1], 2) +
                std::pow(vertices[edge[0]][2] - vertices[edge[1]][2], 2) );
    }
    std::cout << "Calculated totalLength:" << totalLength << "\n";
    std::cout << "Type:" << type << "\n";
    int estimated_use = -1;
    if (type.find("Atrail")) {
        std::cout << "Estimating according to Atrail\n";
        estimated_use = (int)((totalLength * scaling * 2.80) - vertexnr * 18);
    }
    else if (type.find("Spanning tree")) { // Spanning
        estimated_use = (int)((totalLength * scaling * 4) - vertexnr * 18);
    }
    else if (type.find("Scaffold free")) {
        estimated_use = estimated_use = (int)((totalLength * scaling * 2) - vertexnr * 18);
        return std::make_pair(estimated_use/2, vertexnr*4);
    }
    return std::make_pair(estimated_use/2, vertexnr*4);
}

/*void vHelix::physX_relaxation_(const QVector<QVariant> args)
{

     /(double &scaling, bool &discretize_lengths, double &density,
                      double &spring_stiffness, double &fixed_spring_stiffness,
                      double &spring_damping, bool &attach_fixed,
                      double &static_friction, double &dynamic_friction,
                      double &restitution, double &rigid_body_sleep_threshold,
                      std::string &visual_debugger)
    /
    std::stringstream sstr;
    sstr << vHelixFunc::ExePath() << "/../workspace/" << mesh_;
    std::string input = sstr.str();
    std::string output = input;
    output.resize(output.size() - 3);
    output.append("rpoly");
    double *dblArgs = new double[9];
    bool *boolArgs = new bool[3];
    unsigned int k = 0; unsigned int j = 0;
    for (unsigned int i = 0; i < 15; i++) {
        switch (i) {
            case 0: // [0]scaling
            case 2: // [1]density
            case 3: // [2]spring stiffness
            case 4: // [3]fixed spring stiffness
            case 5: // [4]spring damping
            case 7: // [5]static friction
            case 8: // [6]dynamic friction
            case 9: // [7]restitution
            case 10:// [8]rigid body sleep threshold
                dblArgs[k] = args[i].toDouble();
                k++;
                break;
            case 1:  // discretize lengths
            case 6:  // attach fixed
            case 11: // visual debugger
                if (args[i].toString() == "Disabled") {
                    boolArgs[j] = false;
                }
                else {
                    boolArgs[j] = true;
                }
                j++;
                break;
        }
    }
    const int iterations = args[12].toInt();
    relaxation::relax_main(input, output, dblArgs, iterations, boolArgs, *this);
}
void vHelix::physX_relaxation_(const QVector<QVariant> args)
{
    std::stringstream sstr;
    sstr << vHelixFunc::ExePath() << "/../workspace/" << mesh_;
    std::string input = sstr.str();
    std::string output = input;
    output.resize(output.size() - 3);
    output.append("rpoly");
    std::cerr << "Running relaxation to " << output.c_str()<< std::endl;
    design->relax(args);
    sendToConsole_(design->getoutstream());
}*/
