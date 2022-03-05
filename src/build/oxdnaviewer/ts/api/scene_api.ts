/**
 * Bits of code to facilitate querying structures from the browser console
 */
module api{
    export function toggleStrand(strand: Strand): Strand{
        strand.map( 
            (n:Nucleotide) => n.toggleVisibility());

        strand.system.callUpdates(['instanceVisibility'])
        if (tmpSystems.length > 0) {
            tmpSystems.forEach(s => s.callUpdates(['instanceVisibility']));
        }

        render();
        return strand;
    }

    // get a dictionary with every strand length : [strand] listed   
    export function countStrandLength(system = systems[0]) {
        let strandLength : { [index: number]: [Strand] } = {};
        system.strands.map((strand: Strand) => {
            let l = strand.getLength();
            if (l in strandLength) 
                strandLength[l].push(strand);
            else
                strandLength[l] = [strand];
        });
        return strandLength;  
    };

    //highlight
    export function highlight5ps(system = systems[0]){
        system.strands.forEach(strand=>strand.end5.select());
        updateView(system);
        render();
    }

    export function toggleElements(elems: BasicElement[]) {
        let sys = new Set<System>();
        let tmpSys = new Set<System>();
        elems.forEach(e=>{
            e.toggleVisibility();
            sys.add(e.getSystem());
            if (e.dummySys) {
                tmpSys.add(e.dummySys);
            }
        });
        sys.forEach(s=>s.callUpdates(['instanceVisibility']));
        tmpSys.forEach(s=>s.callUpdates(['instanceVisibility']));
        render();
    }

    export function toggleAll(system = systems[0]){
        system.strands.forEach(strand=>strand.forEach(n => n.toggleVisibility()));
        system.callUpdates(['instanceVisibility'])
        if (tmpSystems.length > 0) {
            tmpSystems.forEach ((s) => {
                s.callUpdates(['instanceVisibility'])
            })
        }
        render();
    }

    //toggles the nuceloside colors on and off
    export function toggleBaseColors() {
        elements.forEach(
            (e: BasicElement) => {
                if (e.strand == null) return
                let sys = e.getSystem();
                let sid = e.sid;
                if (e.dummySys !== null) {
                    sys = e.dummySys
                    sid = e.sid;
                }
                //because the precision of the stored color value (32-bit) and defined color value (64-bit) are different,
                //you have to do some weird casting to get them to be comparable.
                let tmp = e.getInstanceParameter3("nsColors") //maybe this shouldn't be a vector3...
                let c = [tmp.x.toPrecision(6), tmp.y.toPrecision(6), tmp.z.toPrecision(6)];
                let g = [GREY.r.toPrecision(6), GREY.g.toPrecision(6), GREY.b.toPrecision(6)];
                if (JSON.stringify(c)==JSON.stringify(g)){
                    let newC = e.elemToColor(e.type);
                    sys.fillVec('nsColors', 3, sid, [newC.r, newC.g, newC.b])
                }
                else {
                    sys.fillVec('nsColors', 3, sid,[GREY.r, GREY.g, GREY.b]);
                }
            }
        )
        for (let i = 0; i < systems.length; i++) {
            systems[i].nucleoside.geometry["attributes"].instanceColor.needsUpdate = true;
        }
        if (tmpSystems.length > 0) {
            tmpSystems.forEach ((s) => {
                s.nucleoside.geometry["attributes"].instanceColor.needsUpdate = true;
            })
        }
        render();
    }
    
    export function trace53(element: BasicElement): BasicElement[]{
        let elems : BasicElement[] = [];
        let c : BasicElement = element; 
        while(c){
            elems.push(c);
            c = c.n3;
        }
        return elems;
    }

    export function trace35(element: BasicElement): BasicElement[]{
        let elems : BasicElement[] = [];
        let c : BasicElement = element; 
        while(c){
            elems.push(c);
            c = c.n5;
        }
        return elems;
    }

    export function getElements(targets: number[]): BasicElement[] {
        let out = [];
        targets.forEach((n) => {
            out.push(elements.get(n));
        });
        return(out);
    }

    export function selectElementIDs(targets: number[], keepPrevious?: boolean) {
        selectElements(getElements(targets), keepPrevious);
    }

    /**
     * Show the specified element in the viewport
     * @param element Element to center view at
     */
    export function findElement(element: BasicElement) {
        let targetPos = element.getInstanceParameter3('bbOffsets');

        // Target trackball controls at element position
        controls.target = targetPos;

        // Move in close to the element
        let targetDist = 3;
        let dist = (camera.position.distanceTo(targetPos));
        camera.position.lerp(targetPos, 1-(targetDist/dist));
    }

    export function selectElements(elems: BasicElement[], keepPrevious?: boolean) {
        if (!keepPrevious) {
            clearSelection();
        }
        elems.forEach(e => {
            if (!selectedBases.has(e)) {
                e.toggle();
            }
        });
        systems.forEach(sys => {
            updateView(sys);
        });
        if (selectedBases.size > 0 && view.transformMode.enabled()) {
            transformControls.show();
        }
    }
    
    //there's probably a less blunt way to do this...
    export function removeColorbar() {
        let l = colorbarScene.children.length;
        for (let i = 0; i < l; i++) {
            if (colorbarScene.children[i].type == "Sprite" || colorbarScene.children[i].type == "Line") {
                colorbarScene.remove(colorbarScene.children[i]);
                i -= 1;
                l -= 1;
            }
        }
        colorbarScene.remove(lut.legend.mesh)
        //reset light to default
        pointlight.intensity = 0.5;
        renderColorbar();
    }

    //turns out that lut doesn't save the sprites so you have to completley remake it
    export function showColorbar() {
        colorbarScene.add(lut.legend.mesh);
        let labels =  lut.setLegendLabels({'title':lut.legend.labels.title, 'ticks':lut.legend.labels.ticks}); //don't ask, lut stores the values but doesn't actually save the sprites anywhere so you have to make them again...
        colorbarScene.add(labels["title"]);
        for (let i = 0; i < Object.keys(labels['ticks']).length; i++) {
            colorbarScene.add(labels['ticks'][i]);
            colorbarScene.add(labels['lines'][i]);
        }
        //colormap doesn't look right unless the light is 100%
        pointlight.intensity = 1.0; 

        renderColorbar();
    }

    export function changeColormap(name: string) {
        if (lut != undefined) {
            api.removeColorbar();
            let key = lut.legend.labels.title;
            let min = lut.minV;
            let max = lut.maxV;
            lut = lut.changeColorMap(name);
            lut.setMax(max);
            lut.setMin(min);
            lut.setLegendOn({ 'layout': 'horizontal', 'position': { 'x': 0, 'y': 0, 'z': 0 }, 'dimensions': { 'width': 2, 'height': 12 } }); //create legend
            lut.setLegendLabels({ 'title': key, 'ticks': 5 });
            for (let i = 0; i < systems.length; i++){
                let system = systems[i];
                let end = system.systemLength()
                for (let i = 0; i < end; i++) { //insert lut colors into lutCols[] to toggle Lut coloring later
                    system.lutCols[i] = lut.getColor(Number(system.colormapFile[key][i]));
                }
            }
            updateColoring();
        }
        else {
            defaultColormap = name;
        }
    }

    export function setColorBounds(min: number, max: number) {
        let key = lut.legend.labels.title;
        lut.setMax(max);
        lut.setMin(min);
        api.removeColorbar();
        lut.setLegendOn({ 'layout': 'horizontal', 'position': { 'x': 0, 'y': 0, 'z': 0 }, 'dimensions': { 'width': 2, 'height': 12 } }); //create legend
        lut.setLegendLabels({ 'title': key, 'ticks': 5 });
        for (let i = 0; i < systems.length; i++){
            let system = systems[i];
            let end = system.systemLength()
            for (let i = 0; i < end; i++) { //insert lut colors into lutCols[] to toggle Lut coloring later
                system.lutCols[i] = lut.getColor(Number(system.colormapFile[key][i]));
            }
        }
        updateColoring();
    }
    
    export function spOnly() {
        elements.forEach((n: BasicElement) => {
            n.setInstanceParameter('scales', [0, 0, 0]);
            n.setInstanceParameter('nsScales', [0, 0, 0]);
            n.setInstanceParameter('conScales', [0, 0, 0]);
        });
        for (let i = 0; i < systems.length; i++) {
            systems[i].callUpdates(['instanceScale'])
        }
        for (let i = 0; i < tmpSystems.length; i++) {
            tmpSystems[i].callUpdates(['instanceScale'])
        }
        render();

    }

    export function showEverything() {
        elements.forEach((n: BasicElement) => {
            n.setInstanceParameter('scales', [1, 1, 1]);
            n.setInstanceParameter('nsScales', [0.7, 0.3, 0.7]);
            n.setInstanceParameter('conScales', [1, n.bbnsDist, 1]);
        });
        for (let i = 0; i < systems.length; i++) {
            systems[i].callUpdates(['instanceScale'])
        }
        for (let i = 0; i < tmpSystems.length; i++) {
            tmpSystems[i].callUpdates(['instanceScale'])
        }
        render();
    }

    export function switchCamera() {
        if (camera instanceof THREE.PerspectiveCamera) {
            //get camera parameters
            const far = camera.far;
            const near = camera.near;
            const focus = controls.target;
            const fov = camera.fov*Math.PI/180; //convert to radians
            const pos = camera.position;
            let width = 2*Math.tan(fov/2)*focus.distanceTo(pos);
            let height = width/camera.aspect;
            const up = camera.up;
            const quat = camera.quaternion;
            let cameraHeading = new THREE.Vector3(0, 0, -1);
            cameraHeading.applyQuaternion(quat);

            //if the camera is upside down, you need to flip the corners of the orthographic box
            if (quat.dot(refQ) < 0 && quat.w > 0) {
                width *= -1;
                height *= -1;
            }
    
            //create a new camera with same properties as old one
            let newCam = createOrthographicCamera(-width/2, width/2, height/2, -height/2, near, far, pos.toArray());
            newCam.up = up
            newCam.lookAt(focus);
            let light = pointlight;
            scene.remove(camera);
            camera = newCam;
            controls.object = camera;
            camera.add(light);
            scene.add(camera);

            document.getElementById("cameraSwitch").innerHTML = "Perspective";
        }
        else if (camera instanceof THREE.OrthographicCamera) {
            //get camera parameters
            const far = camera.far;
            const near = camera.near;
            const focus = controls.target;
            const pos = camera.position;
            const up = camera.up;
            let fov = 2*Math.atan((((camera.right-camera.left)/2))/focus.distanceTo(pos))*180/Math.PI;
            
            //if the camera is upside down, you need to flip the fov for the perspective camera
            if (camera.left > camera.right) {
                fov *= -1
            }

            //create a new camera with same properties as old one
            let newCam = createPerspectiveCamera(fov, near, far, pos.toArray());
            newCam.up = up;
            newCam.lookAt(focus);
            let light = pointlight;
            scene.remove(camera);
            camera = newCam;
            controls.object = camera;
            camera.add(light);
            scene.add(camera);

            document.getElementById("cameraSwitch").innerHTML = "Orthographic";
        }
        render();
    }

    //export function flipConnectors() {
    //    instancedBBconnector.
    //}
}
