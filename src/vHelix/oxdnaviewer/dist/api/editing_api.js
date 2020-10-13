/**
 * Bits of code that handle structure editing
 */
var edit;
(function (edit) {
    /**
         * Split the elemnt's strand at the element provided
         * @param element Element to split at
         * @returns new strand created in split
         */
    function splitStrand(element) {
        const strand = element.strand, sys = strand.system;
        // Splitting a circular strand doesn't make
        // more strands, but it will then no longer
        // be circular.
        if (strand.circular) {
            strand.circular = false;
            return;
        }
        // No need to split if one half will be empty
        if (!element.neighbor5) {
            return;
        }
        // Nucleotides which are after the nick
        const orphans = api.trace35(element);
        strand.excludeElements(orphans);
        // Create, fill and deploy new strand
        let newStrand;
        if (strand.getType() == "Peptide") {
            newStrand = strand.system.createStrand(-strand.system.strands.length);
        }
        else {
            newStrand = strand.system.createStrand(strand.system.strands.length);
        }
        strand.system.addStrand(newStrand);
        let lidCounter = 0;
        orphans.forEach((e) => {
            newStrand.addMonomer(e);
            e.lid = lidCounter;
            lidCounter += 1;
            e.updateColor();
        });
        if (strand.label) {
            newStrand.label = `${strand.label}_2`;
            strand.label = `${strand.label}_1`;
        }
        // Update local ids in the remnant strand
        // If there are dummy systems, you need to rebuild
        // anyway and they need static local IDs
        if (tmpSystems.length == 0) {
            let i = 0;
            strand.monomers.forEach((n) => {
                n.lid = i++;
            });
        }
        sys.callUpdates(['instanceColor']);
        return newStrand;
    }
    function insert(e, sequence) {
        let edits = [];
        let end5 = e;
        let end3 = e.neighbor3;
        if (!end3) {
            notify("Cannot insert at the end of a strand, use extend instead");
            return;
        }
        // Shorthand to get the backbone position
        const getPos = e => { return e.getInstanceParameter3("bbOffsets"); };
        // Nick between the elements
        let nicking = new RevertableNick(end5);
        edits.push(nicking);
        nicking.do();
        // Extend, move and make sure to be revertable
        const added = extendStrand(end5, sequence);
        const firstPos = getPos(end5);
        const lastPos = getPos(end3);
        added.forEach((e, i) => {
            // Position on line between e1 and e2
            let newPos = firstPos.clone().lerp(lastPos, (i + 1) / (added.length + 1));
            translateElements(new Set([e]), newPos.sub(getPos(e)));
        });
        let instanceCopies = added.map(e => { return new InstanceCopy(e); });
        // Get center of mass so that we can place them back if reverted
        let pos = new THREE.Vector3();
        added.forEach(e => pos.add(getPos(e)));
        pos.divideScalar(added.length);
        let addition = new RevertableAddition(instanceCopies, added, pos);
        edits.push(addition);
        // Finally, ligate the last added element to e2
        const lastAdded = added[added.length - 1];
        let ligation = new RevertableLigation(lastAdded, end3);
        edits.push(ligation);
        ligation.do();
        editHistory.add(new RevertableMultiEdit(edits));
        return added;
    }
    edit.insert = insert;
    function skip(elems) {
        let edits = [];
        elems.forEach(e => {
            let e3 = e.neighbor3;
            let e5 = e.neighbor5;
            let deletion = new RevertableDeletion([e]);
            edits.push(deletion);
            deletion.do();
            // Only ligate if it has both neigbors
            if (e3 && e5) {
                let ligation = new RevertableLigation(e3, e5);
                edits.push(ligation);
                ligation.do();
            }
        });
        editHistory.add(new RevertableMultiEdit(edits));
    }
    edit.skip = skip;
    function nick(element) {
        let sys = element.getSystem(), sid = element.gid - sys.globalStartId;
        if (element.dummySys !== null) {
            sys = element.dummySys;
            sid = element.sid;
        }
        // we break connection to the 3' neighbor 
        let neighbor = element.neighbor3;
        element.neighbor3 = null;
        neighbor.neighbor5 = null;
        splitStrand(element);
        sys.fillVec('bbconScales', 3, sid, [0, 0, 0]);
        sys.bbconnector.geometry["attributes"].instanceScale.needsUpdate = true;
        render();
    }
    edit.nick = nick;
    function ligate(element1, element2) {
        let end5, end3;
        //find out which is the 5' end and which is 3'
        if (element1.neighbor5 == null && element2.neighbor3 == null) {
            end5 = element1;
            end3 = element2;
        }
        else if (element1.neighbor3 == null && element2.neighbor5 == null) {
            end5 = element2;
            end3 = element1;
        }
        else {
            notify("Please select one nucleotide with an available 3' connection and one with an available 5'");
            return;
        }
        // strand1 will have an open 5' and strand2 will have an open 3' end
        // strand2 will be merged into strand1
        let sys5 = end5.getSystem(), sys3 = end3.getSystem(), strand1 = end5.strand, strand2 = end3.strand;
        // handle strand1 and strand2 not being in the same system
        if (sys5 !== sys3) {
            let tmpSys = new System(tmpSystems.length, 0);
            tmpSys.initInstances(strand2.monomers.length);
            for (let i = 0, len = strand2.monomers.length; i < len; i++) {
                copyInstances(strand2.monomers[i], i, tmpSys);
                strand2.monomers[i].setInstanceParameter('visibility', [0, 0, 0]);
                strand2.monomers[i].dummySys = tmpSys;
                strand2.monomers[i].sid = i;
            }
            sys3.callUpdates(['instanceVisibility']);
            addSystemToScene(tmpSys);
            tmpSystems.push(tmpSys);
        }
        // lets orphan strand2 element
        let bases2 = [...strand2.monomers]; // clone the references to the elements
        strand2.excludeElements(strand2.monomers);
        //check that it is not the same strand
        if (strand1 !== strand2) {
            //remove strand2 object 
            strand2.system.removeStrand(strand2);
        }
        else {
            strand1.circular = true;
        }
        // Strand id update
        let strID = 1;
        sys5.strands.forEach((strand) => strand.strandID = strID++);
        if (sys3 !== sys5) {
            sys3.strands.forEach((strand) => strand.strandID = strID++);
        }
        // and add them back into strand1 
        //create fill and deploy new strand 
        let i = end5.lid + 1;
        bases2.forEach((n) => {
            strand1.addMonomer(n);
            n.lid = i;
            i++;
        });
        //since strand IDs were updated, we also need to update the coloring
        updateColoring();
        //connect the 2 element objects 
        end5.neighbor5 = end3;
        end3.neighbor3 = end5;
        //last, add the sugar-phosphate bond
        let p2 = end3.getInstanceParameter3("bbOffsets");
        let xbb = p2.x, ybb = p2.y, zbb = p2.z;
        let p1 = end5.getInstanceParameter3("bbOffsets");
        let xbbLast = p1.x, ybbLast = p1.y, zbbLast = p1.z;
        let xsp = (xbb + xbbLast) / 2, ysp = (ybb + ybbLast) / 2, zsp = (zbb + zbbLast) / 2;
        let spLen = Math.sqrt(Math.pow(xbb - xbbLast, 2) + Math.pow(ybb - ybbLast, 2) + Math.pow(zbb - zbbLast, 2));
        let spRotation = new THREE.Quaternion().setFromUnitVectors(new THREE.Vector3(0, 1, 0), new THREE.Vector3(xsp - xbb, ysp - ybb, zsp - zbb).normalize());
        end3.setInstanceParameter('bbconOffsets', [xsp, ysp, zsp]);
        end3.setInstanceParameter('bbconRotation', [spRotation.w, spRotation.z, spRotation.y, spRotation.x]);
        end3.setInstanceParameter('bbconScales', [1, spLen, 1]);
        sys5.callUpdates(["instanceOffset", "instanceScale", "instanceColor", "instanceRotation", "instanceVisibility"]);
        sys3.callUpdates(["instanceOffset", "instanceScale", "instanceColor", "instanceRotation", "instanceVisibility"]);
        if (tmpSystems.length > 0) {
            tmpSystems.forEach((s) => {
                s.callUpdates(['instanceOffset', 'instanceRotation', 'instanceScale', 'instanceColor', 'instanceVisibility']);
            });
        }
        render();
    }
    edit.ligate = ligate;
    /**
     *
     * @param victims
     */
    function deleteElements(victims) {
        let needsUpdateList = new Set();
        victims.forEach((e) => {
            let sys;
            let strand = e.strand;
            if (e.dummySys !== null) {
                sys = e.dummySys;
            }
            else {
                sys = e.getSystem();
            }
            needsUpdateList.add(sys);
            let newStrand;
            // Split strand if we won't also delete further downstream
            if (e.neighbor3 && !victims.includes(e.neighbor3)) {
                newStrand = splitStrand(e);
            }
            if (e.neighbor3) {
                e.neighbor3.neighbor5 = null;
                e.neighbor3 = null;
            }
            if (e.neighbor5) {
                // If different systems, we need to update both
                let n5sys = e.neighbor5.dummySys ? e.neighbor5.dummySys : e.neighbor5.getSystem();
                needsUpdateList.add(n5sys);
                e.neighbor5.neighbor3 = null;
                e.neighbor5.setInstanceParameter("bbconScales", [0, 0, 0]);
                e.neighbor5 = null;
            }
            e.toggleVisibility();
            e.strand.excludeElements([e]);
            elements.delete(e.gid);
            selectedBases.delete(e);
            // Remove strand(s) if empty
            if (strand.isEmpty()) {
                let s = strand.system;
                s.removeStrand(strand);
                // Remove system if empty
                if (s.isEmpty()) {
                    systems.splice(systems.indexOf(s), 1);
                    sysCount--;
                }
            }
            if (newStrand && newStrand != strand && newStrand && newStrand.isEmpty()) {
                let s = newStrand.system;
                s.removeStrand(newStrand);
                // Remove system if empty
                if (s.isEmpty()) {
                    systems.splice(systems.indexOf(s), 1);
                    sysCount--;
                }
            }
        });
        needsUpdateList.forEach((s) => {
            s.callUpdates(['instanceVisibility', 'instanceScale', 'instanceColor']);
        });
        render();
    }
    edit.deleteElements = deleteElements;
    /**
     * Add elements from saved instance copies, at specified position
     * @param instCopies Instance copies of elements to add
     * @param pos Intended position of elements center of mass
     */
    function addElementsAt(instCopies, pos) {
        // Add elems
        let elems = addElements(instCopies);
        if (pos) {
            // Calculate elems center of mass
            let com = new THREE.Vector3();
            elems.forEach(e => {
                let p = e.getPos();
                com.add(p);
            });
            com.divideScalar(elems.length);
            // Move elements to position
            translateElements(new Set(elems), pos.clone().sub(com));
        }
        return elems;
    }
    edit.addElementsAt = addElementsAt;
    /**
     * Add elements from saved instance copies
     * @param instCopies Instance copies of elements to add
     */
    function addElements(instCopies) {
        // Initialize a dummy system to put the monomers in
        const tmpSys = new System(systems.length, 0);
        tmpSys.initInstances(instCopies.length);
        tmpSystems.push(tmpSys);
        let oldgids = instCopies.map(c => { return c.gid; });
        let elems = instCopies.map((c, sid) => {
            // Create new element
            let e = new c.elemType(undefined, undefined);
            // Give back the old copied gid if it's not already in use,
            // otherwise, create a new one
            if (!elements.has(c.gid)) {
                elements.set(c.gid, e);
            }
            else {
                elements.push(e);
            }
            c.writeToSystem(sid, tmpSys);
            e.dummySys = tmpSys;
            e.sid = sid;
            e.type = c.type;
            // Assign a picking color
            let idColor = new THREE.Color();
            idColor.setHex(e.gid + 1); //has to be +1 or you can't grab nucleotide 0
            tmpSys.fillVec('bbLabels', 3, sid, [idColor.r, idColor.g, idColor.b]);
            return e;
        });
        addSystemToScene(tmpSys);
        let toLigate = [];
        // Sort out neighbors
        elems.forEach((e, sid) => {
            let c = instCopies[sid];
            // Add neighbors to new copies in list, or to existing elements
            // if they don't already have neighbors
            if (c.n3gid >= 0) { // If we have a 3' neighbor
                let i3 = oldgids.findIndex(gid => { return gid == c.n3gid; });
                // If the 3' neighbor is also about to be added, we link to
                // the new object instead
                if (i3 >= 0) {
                    e.neighbor3 = elems[i3];
                    elems[i3].neighbor5 = e;
                    // Otherwise, if the indicated neighbor exists and we can link
                    // the new element to it without overwriting anything
                }
                else if (elements.has(c.n3gid) &&
                    elements.get(c.n3gid) &&
                    !elements.get(c.n3gid).neighbor5) {
                    e.neighbor3 = null;
                    toLigate.push([e, elements.get(c.n3gid)]);
                    //e.neighbor3 = elements.get(c.n3gid);
                    //e.neighbor3.neighbor5 = e;
                    // If not, we don't set any neighbor
                }
                else {
                    e.neighbor3 = null;
                }
            }
            // Same as above, but for 5'
            if (c.n5gid >= 0) { // If we have a 5' neighbor
                let i5 = oldgids.findIndex(gid => { return gid == c.n5gid; });
                // If the 5' neighbor is also about to be added, we link to
                // the new object instead
                if (i5 >= 0) {
                    e.neighbor5 = elems[i5];
                    elems[i5].neighbor3 = e;
                    // Otherwise, if the indicated neighbor exists and we can link
                    // the new element to it without overwriting anything
                }
                else if (elements.has(c.n5gid) &&
                    elements.get(c.n5gid) &&
                    !elements.get(c.n5gid).neighbor3) {
                    e.neighbor5 = null;
                    toLigate.push([e, elements.get(c.n5gid)]);
                    // If not, we don't set any neighbor
                }
                else {
                    e.neighbor5 = null;
                }
            }
        });
        // Sort out strands
        elems.forEach((e, sid) => {
            let c = instCopies[sid];
            let sys = c.system;
            let circular;
            // Do we have a strand assigned already?
            if (!e.strand) {
                // Does any of our neighbors know what strand this is?
                let i = e;
                while (!i.strand) { // Look in 3' dir
                    if (e === i.neighbor3) {
                        circular = true;
                        break;
                    }
                    if (i.neighbor3)
                        i = i.neighbor3;
                    else
                        break;
                }
                if (!i.strand) { // If nothing, look in 5' dir
                    i = e;
                    while (!i.strand) {
                        if (e === i.neighbor5) {
                            circular = true;
                            break;
                        }
                        if (i.neighbor5)
                            i = i.neighbor5;
                        else
                            break;
                    }
                }
                // If we found something
                if (i.strand) {
                    // Add us to the strand
                    i.strand.addMonomer(e);
                }
                else {
                    // Create a new strand
                    // This is ugly, but we need to find the next available
                    // strand ID and we can't do sys.strands.length because
                    // of proteins
                    let sMin = 1;
                    let sMax = -1;
                    sys.strands.forEach(s => {
                        sMin = Math.min(sMin, s.strandID);
                        sMax = Math.max(sMax, s.strandID);
                    });
                    let newStrandID;
                    if (e.isAminoAcid()) {
                        newStrandID = sMin - 1;
                    }
                    else {
                        newStrandID = sMax + 1;
                    }
                    let strand = sys.createStrand(newStrandID);
                    sys.addStrand(strand);
                    strand.addMonomer(e);
                }
                if (circular) {
                    e.strand.circular = true;
                }
            }
            // If the whole system has been removed we have to add it back again
            if (!systems.includes(sys)) {
                systems.push(sys);
            }
        });
        // Update bonds
        elems.forEach(e => {
            // Do we still have a 3' neighbor?
            if (e.neighbor3) {
                // Update backbone bond
                calcsp(e);
            }
            else {
                // Set explicitly to null
                e.neighbor3 = null;
                // Remove backbone bond
                tmpSys.fillVec('bbconScales', 3, e.sid, [0, 0, 0]);
                tmpSys.bbconnector.geometry["attributes"].instanceScale.needsUpdate = true;
                render();
            }
            if (!e.neighbor5) {
                // Set explicitly to null
                e.neighbor5 = null;
            }
            e.updateColor();
        });
        tmpSys.callUpdates(['instanceColor']);
        toLigate.forEach(p => {
            ligate(p[0], p[1]);
        });
        return elems;
    }
    edit.addElements = addElements;
    function addElementsBySeq(end, sequence, tmpSys, direction, inverse, lidCounter) {
        // add monomers to the strand
        const strand = end.strand;
        const lines = end.extendStrand(sequence.length, direction, false);
        let last = end;
        let addedElems = [];
        //create topology
        for (let i = 0, len = sequence.length; i < len; i++) {
            let e = strand.createBasicElement();
            elements.push(e); // Add element and assign gid
            e.lid = lidCounter;
            e.sid = lidCounter; //You're always adding to a tmpSys so this is needed
            e.dummySys = tmpSys;
            last[direction] = e;
            e[inverse] = last;
            e.type = sequence[i];
            strand.addMonomer(e);
            last = e;
            lidCounter++;
            addedElems.push(e);
        }
        // Make last element end of strand
        last[direction] = null;
        let e = end[direction];
        //position new monomers
        for (let i = 0, len = sequence.length; i < len; i++) {
            e.calcPositionsFromConfLine(lines[i]);
            e = e[direction];
        }
        strand.circular = false;
        addSystemToScene(tmpSys);
        //putting this in one loop would slow down loading systems
        //would require dereferencing the backbone position of every nucleotide
        //its not worth slowing down everything to avoid this for loop
        //which is much more of an edge case anyway.
        e = end;
        while (e && e[direction]) {
            // Backbone must be drawn from 5' end
            if (direction == "neighbor5") {
                calcsp(e.neighbor5);
            }
            else {
                calcsp(e);
            }
            e = e[direction];
        }
        return addedElems;
    }
    function addDuplexBySeq(end, sequence, tmpSys, direction, inverse, lidCounter) {
        // variables ending in "2" correspond to complement strand
        let end2 = end.findPair();
        const strand = end.strand;
        const strand2 = end2.strand;
        const l = sequence.length;
        const lines = end.extendStrand(l, direction, true); // true = double strand
        let last1 = end;
        let last2 = end2;
        let addedElems = [];
        for (let i = 0; i < l; i++) {
            let e1 = strand.createBasicElement();
            elements.push(e1);
            e1.lid = lidCounter + i;
            e1.sid = lidCounter + i;
            e1.dummySys = tmpSys;
            last1[direction] = e1;
            e1[inverse] = last1;
            e1.type = sequence[i];
            strand.addMonomer(e1);
            last1 = e1;
            addedElems.push(e1);
        }
        for (let i = 0; i < l; i++) {
            let e1 = addedElems[i];
            let e2 = strand2.createBasicElement();
            elements.push(e2);
            e2.lid = lidCounter + i;
            e2.sid = lidCounter + l + i;
            e2.dummySys = tmpSys;
            last2[inverse] = e2;
            e2[direction] = last2;
            e2.type = e1.getComplementaryType();
            strand2.addMonomer(e2);
            last2 = e2;
            addedElems.push(e2);
            e1.pair = e2;
            e2.pair = e1;
        }
        last1[direction] = null;
        last2[inverse] = null;
        let e1 = end[direction];
        let e2 = end2[inverse];
        for (let i = 0; i < l; i++) {
            e1.calcPositionsFromConfLine(lines[i]);
            e1 = e1[direction];
        }
        // complementary strand adds elements in reverse direction
        for (let i = l * 2 - 1; i >= l; i--) {
            e2.calcPositionsFromConfLine(lines[i]);
            e2 = e2[inverse];
        }
        strand.circular = false;
        strand2.circular = false;
        addSystemToScene(tmpSys);
        e1 = end;
        e2 = end2;
        while (e1 && e1[direction]) {
            if (direction == "neighbor5") {
                calcsp(e1.neighbor5);
            }
            else {
                calcsp(e1);
            }
            e1 = e1[direction];
        }
        while (e2 && e2[inverse]) {
            if (inverse == "neighbor5") {
                calcsp(e2.neighbor5);
            }
            else {
                calcsp(e2);
            }
            e2 = e2[inverse];
        }
        render();
        return addedElems;
    }
    /**
     * Create new monomers extending from the provided one.
     * @param end
     * @param sequence
     */
    function extendStrand(end, sequence) {
        // figure out which way we're going
        let direction;
        let inverse;
        if (end.neighbor3 == null) {
            direction = "neighbor3";
            inverse = "neighbor5";
        }
        else if (end.neighbor5 == null) {
            direction = "neighbor5";
            inverse = "neighbor3";
        }
        else {
            notify("Please select a monomer that has an open neighbor");
            return;
        }
        // initialize a dummy system to put the monomers in
        const tmpSys = new System(tmpSystems.length, 0);
        tmpSys.initInstances(sequence.length);
        tmpSystems.push(tmpSys);
        let addedElems = addElementsBySeq(end, sequence, tmpSys, direction, inverse, 0);
        render();
        return addedElems;
    }
    edit.extendStrand = extendStrand;
    /**
     * Create double helix of monomers extending from provided helix
     * @param end
     * @param sequence
     */
    function extendDuplex(end, sequence) {
        let end2 = end.findPair();
        // create base pair if end doesn't have one alreadyl
        let addedElems = [];
        if (!end2) {
            end2 = createBP(end);
            addedElems.push(end2);
        }
        let direction;
        let inverse;
        if (end.neighbor5 == null && end2.neighbor3 == null) {
            direction = "neighbor5";
            inverse = "neighbor3";
        }
        else if (end.neighbor3 == null && end2.neighbor5 == null) {
            direction = "neighbor3";
            inverse = "neighbor5";
        }
        else {
            notify("Please select a monomer that has an open neighbor");
            return;
        }
        // initialize dummy systems to put each sequence in
        const tmpSys = new System(tmpSystems.length, 0);
        tmpSys.initInstances(sequence.length * 2);
        tmpSystems.push(tmpSys);
        addedElems = addedElems.concat(addDuplexBySeq(end, sequence, tmpSys, direction, inverse, 0));
        render();
        return addedElems;
    }
    edit.extendDuplex = extendDuplex;
    function setSequence(elems, sequence, setComplementaryBases) {
        setComplementaryBases = setComplementaryBases || false;
        if (elems.length != sequence.length) {
            notify(`You have ${elems.length} particles selected and ${sequence.length} letters in the sequence...doing my best`);
        }
        // Sort elements by their id, in 5' to 3' order
        elems.sort((a, b) => { return a.lid < b.lid ? 1 : -1; });
        // Define a function to satisfy view.longCalculation callback
        let set = function () {
            let len = Math.min(elems.length, sequence.length);
            for (let i = 0; i < len; i++) {
                elems[i].changeType(sequence[i]);
                if (setComplementaryBases) {
                    let paired = elems[i].pair;
                    if (paired) {
                        paired.changeType(elems[i].getComplementaryType());
                    }
                }
            }
            systems.concat(tmpSystems).forEach(system => {
                system.nucleoside.geometry["attributes"].instanceColor.needsUpdate = true;
                system.callUpdates(['instanceColor']);
            });
            render();
        };
        // If we need to find basepairs, do that first and wait
        // for the calculation to finish. Otherwise, set the
        // sequence immediately.
        if (setComplementaryBases && !elems[0].isPaired()) {
            view.longCalculation(findBasepairs, view.basepairMessage, set);
        }
        else {
            set();
        }
    }
    edit.setSequence = setSequence;
    /**
     * Creates a new strand with the provided sequence
     * @param sequence
     */
    function createStrand(sequence, createDuplex, isRNA) {
        if (sequence.includes('U')) {
            isRNA = true;
        }
        // Assume the input sequence is 5' -> 3',
        // but oxDNA is 3' -> 5', so we reverse it.
        let tmp = sequence.split("");
        tmp = tmp.reverse();
        sequence = tmp.join("");
        // Initialize a dummy system to put the monomers in 
        const tmpSys = new System(tmpSystems.length, 0);
        tmpSys.initInstances(sequence.length * (createDuplex ? 2 : 1)); // Need to be x2 if duplex
        tmpSystems.push(tmpSys);
        // The strand gets added to the last-added system.
        // Or make a new system if you're crazy and trying to build something from scratch
        let realSys;
        if (systems.length > 0) {
            realSys = systems.slice(-1)[0];
        }
        else {
            realSys = new System(sysCount++, elements.getNextId());
            realSys.initInstances(0);
            systems.push(realSys);
            addSystemToScene(realSys);
            // This is ugly, but if we don't have a box, everything will be
            // squashed into the origin when centering.
            box = new THREE.Vector3(1000, 1000, 1000);
        }
        // Create a new strand
        let strand = realSys.createStrand(realSys.strands.length);
        realSys.addStrand(strand);
        // Initialise proper nucleotide
        let e = isRNA ?
            new RNANucleotide(undefined, strand) :
            new DNANucleotide(undefined, strand);
        let addedElems = [];
        elements.push(e); // Add element and assign gid
        e.dummySys = tmpSys;
        e.lid = 0;
        e.sid = 0;
        e.type = sequence[0];
        e.neighbor3 = null;
        strand.addMonomer(e);
        addedElems.push(e);
        // Place the new strand 10 units in front of the camera
        // with its a1 vector parallel to the camera heading
        // and a3 the cross product of the a1 vector and the camera's up vector
        let cameraHeading = new THREE.Vector3(0, 0, -1);
        cameraHeading.applyQuaternion(camera.quaternion);
        let pos = camera.position.clone().add(cameraHeading.clone().multiplyScalar(20));
        let a3 = new THREE.Vector3;
        a3.crossVectors(cameraHeading, camera.up);
        let line = [pos.x, pos.y, pos.z, cameraHeading.x, cameraHeading.y, cameraHeading.z, a3.x, a3.y, a3.z];
        e.calcPositionsFromConfLine(line);
        e.dummySys = tmpSys;
        // Extends the strand 3'->5' with the rest of the sequence
        // and return all added elements.
        if (createDuplex) {
            // create base pair if end doesn't have one already
            if (!e.findPair()) {
                e.pair = createBP(e);
                addedElems.push(e.pair);
            }
            addedElems = addedElems.concat(addDuplexBySeq(e, sequence.substring(1), tmpSys, "neighbor5", "neighbor3", 1));
        }
        else {
            addedElems = addedElems.concat(addElementsBySeq(e, sequence.substring(1), tmpSys, "neighbor5", "neighbor3", 1));
        }
        return addedElems;
    }
    edit.createStrand = createStrand;
    /**
     * Creates complementary base pair for an element.
     * @param elem
     */
    function createBP(elem, undoable) {
        if (elem.findPair()) {
            notify("Element already has a base pair");
            return;
        }
        // Similar to createStrand
        // Initialize dummy system to put the monomer in
        const tmpSys = new System(tmpSystems.length, 0);
        tmpSys.initInstances(1);
        tmpSystems.push(tmpSys);
        const realSys = systems.slice(-1)[0];
        const strand = realSys.createStrand(realSys.strands.length);
        realSys.addStrand(strand);
        // Add element and assign gid
        const e = new DNANucleotide(undefined, strand);
        elements.push(e);
        e.dummySys = tmpSys;
        e.lid = 0;
        e.sid = 0;
        e.type = elem.getComplementaryType();
        e.neighbor3 = null;
        e.neighbor5 = null;
        e.pair = elem;
        elem.pair = e;
        strand.addMonomer(e);
        const cm = elem.getPos();
        const a1 = elem.getA1();
        const a3 = elem.getA3();
        // calculate position of base pair
        a1.negate();
        a3.negate();
        const pos = cm.clone().sub(a1.clone().multiplyScalar(1.2));
        const line = [pos.x, pos.y, pos.z, a1.x, a1.y, a1.z, a3.x, a3.y, a3.z];
        e.calcPositionsFromConfLine(line);
        e.dummySys = tmpSys;
        addSystemToScene(tmpSys);
        // Add to history, but we only want this if it is a atomic edit
        if (undoable) {
            const instanceCopy = [new InstanceCopy(e)];
            const newCm = e.getPos();
            const position = new THREE.Vector3(newCm.x, newCm.y, newCm.z);
            editHistory.add(new RevertableAddition(instanceCopy, [e], position));
        }
        topologyEdited = true;
        return e;
    }
    edit.createBP = createBP;
    /**
     * Copies the instancing data from a particle to a new system
     * @param source Element to copy from
     * @param id The element's system ID
     * @param destination Destination system
     */
    function copyInstances(source, id, destination) {
        destination.fillVec('cmOffsets', 3, id, source.getInstanceParameter3('cmOffsets').toArray());
        destination.fillVec('bbOffsets', 3, id, source.getInstanceParameter3('bbOffsets').toArray());
        destination.fillVec('nsOffsets', 3, id, source.getInstanceParameter3('nsOffsets').toArray());
        destination.fillVec('nsRotation', 4, id, source.getInstanceParameter4('nsRotation').toArray());
        destination.fillVec('conOffsets', 3, id, source.getInstanceParameter3('conOffsets').toArray());
        destination.fillVec('conRotation', 4, id, source.getInstanceParameter4('conRotation').toArray());
        destination.fillVec('bbconOffsets', 3, id, source.getInstanceParameter3('bbconOffsets').toArray());
        destination.fillVec('bbconRotation', 4, id, source.getInstanceParameter4('bbconRotation').toArray());
        destination.fillVec('bbColors', 3, id, source.getInstanceParameter3('bbColors').toArray());
        destination.fillVec('scales', 3, id, source.getInstanceParameter3('scales').toArray());
        destination.fillVec('nsScales', 3, id, source.getInstanceParameter3('nsScales').toArray());
        destination.fillVec('conScales', 3, id, source.getInstanceParameter3('conScales').toArray());
        destination.fillVec('bbconScales', 3, id, source.getInstanceParameter3('bbconScales').toArray());
        destination.fillVec('visibility', 3, id, source.getInstanceParameter3('visibility').toArray());
        destination.fillVec('nsColors', 3, id, source.getInstanceParameter3('nsColors').toArray());
        destination.fillVec('bbLabels', 3, id, source.getInstanceParameter3('bbLabels').toArray());
    }
})(edit || (edit = {}));
