/**
 * Bits of code that handle structure editing
 */
var edit;
(function (edit) {
    /**
         * Split the element's strand at the element provided
         * @param e Element to split at
         * @returns new strand created in split
         */
    function splitStrand(e) {
        const strand = e.strand;
        // Splitting a circular strand doesn't make more strands.
        // We only need to update endpoints.
        if (strand.isCircular()) {
            e.n3.n5 = null;
            e.n3 = null;
            strand.setFrom(e);
            // Remove connector geometry.
            e.setInstanceParameter("bbconScales", [0, 0, 0]);
            let sys = e.dummySys ? e.dummySys : e.getSystem();
            sys.callUpdates(['instanceVisibility', 'instanceScale', 'instanceColor']);
            return;
        }
        // No need to split if one half will be empty
        if (!e.n3) {
            console.assert(e == e.strand.end3, "Incorrect endpoint");
            return;
        }
        // Create, fill and deploy new strand
        let newStrand;
        if (strand.isPeptide()) {
            newStrand = strand.system.addNewPeptideStrand();
        }
        else {
            newStrand = strand.system.addNewNucleicAcidStrand();
        }
        let tmpn3 = e.n3; // Save 3' neighbour temporarly.
        // Fix endpoints
        // 3'--strand--e.n3 | e--newStrand--5'
        e.n3.n5 = null;
        e.n3 = null;
        strand.setFrom(e);
        newStrand.setFrom(tmpn3);
        [strand, newStrand].forEach(s => {
            console.assert(!s.end3.n3, "Incorrect endpoint after split");
            console.assert(!s.end5.n5, "Incorrect endpoint after split");
        });
        newStrand.forEach(i => {
            console.assert(i.strand == newStrand, "Incorrect strand reference");
            i.updateColor();
        });
        if (strand.label) {
            newStrand.label = `${strand.label}_2`;
            strand.label = `${strand.label}_1`;
        }
        // Remove connector geometry.
        // If different systems, we need to update both
        e.setInstanceParameter("bbconScales", [0, 0, 0]);
        let sys = e.dummySys ? e.dummySys : e.getSystem();
        sys.callUpdates(['instanceVisibility', 'instanceScale', 'instanceColor']);
        return newStrand;
    }
    function move_to(e, to_displace) {
        //displace a list of elements tovards the position of e
        let origin = to_displace[0].getPos();
        let displace_by = e.getPos().sub(origin);
        to_displace.forEach(p => {
            p.translatePosition(displace_by);
        });
        systems.forEach((s) => s.callUpdates(['instanceOffset']));
        tmpSystems.forEach((s) => s.callUpdates(['instanceOffset']));
        render();
    }
    edit.move_to = move_to;
    function insert(e, sequence) {
        let edits = [];
        let end5 = e;
        let end3 = e.n3;
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
            let e3 = e.n3;
            let e5 = e.n5;
            let deletion = new RevertableDeletion([e]);
            edits.push(deletion);
            deletion.do();
            // Only ligate if it has both neighbors
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
        splitStrand(element);
        render();
    }
    edit.nick = nick;
    function ligate(a, b) {
        let end5, end3;
        //find out which is the 5' end and which is 3'
        if (!a.n5 && !b.n3) {
            end5 = a;
            end3 = b;
        }
        else if (!a.n3 && !b.n5) {
            end5 = b;
            end3 = a;
        }
        else {
            notify("Please select one nucleotide with an available 3' connection and one with an available 5'");
            return;
        }
        // strand5 will have an open 5' and strand3 will have an open 3' end
        // strand3 will be merged into strand5
        let sys5 = end5.getSystem(), sys3 = end3.getSystem(), strand5 = end5.strand, strand3 = end3.strand;
        // handle strand1 and strand2 not being in the same system
        if (sys5 !== sys3) {
            let tmpSys = new System(tmpSystems.length, 0);
            tmpSys.initInstances(strand3.getLength());
            strand3.forEach((e, i) => {
                copyInstances(e, i, tmpSys);
                e.setInstanceParameter('visibility', [0, 0, 0]);
                e.dummySys = tmpSys;
            }, true);
            sys3.callUpdates(['instanceVisibility']);
            addSystemToScene(tmpSys);
            tmpSystems.push(tmpSys);
        }
        //connect the 2 element objects 
        end5.n5 = end3;
        end3.n3 = end5;
        // Update 5' end to include the new elements
        strand5.updateEnds();
        strand3.forEach(e => {
            e.strand = strand5;
        });
        //check that it is not the same strand
        if (strand5 !== strand3) {
            //remove strand3 object 
            strand3.system.removeStrand(strand3);
            //since strand IDs were updated, we also need to update the coloring
            updateColoring();
        }
        //last, add the sugar-phosphate bond
        let p2 = end3.getInstanceParameter3("bbOffsets");
        let p1 = end5.getInstanceParameter3("bbOffsets");
        let sp = p2.clone().add(p1).divideScalar(2);
        let spLen = p2.distanceTo(p1);
        let spRotation = new THREE.Quaternion().setFromUnitVectors(new THREE.Vector3(0, 1, 0), sp.clone().sub(p2).normalize());
        end3.setInstanceParameter('bbconOffsets', sp.toArray());
        end3.setInstanceParameter('bbconRotation', [spRotation.w, spRotation.z, spRotation.y, spRotation.x]);
        end3.setInstanceParameter('bbconScales', [1, spLen, 1]);
        sys5.callUpdates(["instanceOffset", "instanceScale", "instanceColor", "instanceRotation", "instanceVisibility"]);
        sys3.callUpdates(["instanceOffset", "instanceScale", "instanceColor", "instanceRotation", "instanceVisibility"]);
        tmpSystems.forEach((s) => {
            s.callUpdates(['instanceOffset', 'instanceRotation', 'instanceScale', 'instanceColor', 'instanceVisibility']);
        });
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
            e = elements.get(e.id); // If we add element, then remove it, then undo both edits
            let sys = e.dummySys ? e.dummySys : e.getSystem();
            let strand = e.strand;
            let n3 = e.n3;
            let n5 = e.n5;
            needsUpdateList.add(sys);
            // Hide element
            e.toggleVisibility();
            // Remove connector geometry.
            if (n5) {
                n5.setInstanceParameter("bbconScales", [0, 0, 0]);
            }
            let strand3, strand5;
            // Split strand if we won't also delete further downstream
            if (n3) {
                strand3 = splitStrand(e);
            }
            // Split strand if we won't also delete further downstream
            if (n5) {
                strand5 = splitStrand(n5);
            }
            // Remove e from element map and selection
            elements.delete(e.id);
            selectedBases.delete(e);
            // Strand 5 should now contain e (and any other neighbours to delete).
            // Remove the strand it only contains e.
            [strand, strand3, strand5].forEach(s => {
                if (s && e == s.end3 && e == s.end5) {
                    let system = s.system;
                    system.removeStrand(s);
                    // Remove system if empty
                    if (system.isEmpty()) {
                        systems.splice(systems.indexOf(system), 1);
                        sysCount--;
                    }
                }
            });
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
        let newClusterMap = new Map();
        let oldids = instCopies.map(c => { return c.id; });
        let elems = instCopies.map((c, sid) => {
            // Create new element
            let e = new c.elemType(undefined, undefined);
            // Give back the old copied id if it's not already in use,
            // otherwise, create a new one
            if (!elements.has(c.id)) {
                elements.set(c.id, e);
            }
            else {
                elements.push(e);
            }
            c.writeToSystem(sid, tmpSys);
            e.dummySys = tmpSys;
            e.sid = sid;
            e.type = c.type;
            // Add pasted elements to new cluster
            // (or clusters, if the copied elements had more than one cluster)
            if (c.clusterId !== undefined && c.clusterId > 0) {
                if (!newClusterMap.has(c.clusterId)) {
                    newClusterMap.set(c.clusterId, ++clusterCounter);
                }
                e.clusterId = newClusterMap.get(c.clusterId);
            }
            // Assign a picking color
            let idColor = new THREE.Color();
            idColor.setHex(e.id + 1); //has to be +1 or you can't grab nucleotide 0
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
            if (c.n3id >= 0) { // If we have a 3' neighbor
                let i3 = oldids.findIndex(id => { return id == c.n3id; });
                // If the 3' neighbor is also about to be added, we link to
                // the new object instead
                if (i3 >= 0) {
                    e.n3 = elems[i3];
                    elems[i3].n5 = e;
                    // Otherwise, if the indicated neighbor exists and we can link
                    // the new element to it without overwriting anything
                }
                else if (elements.has(c.n3id) &&
                    elements.get(c.n3id) &&
                    !elements.get(c.n3id).n5) {
                    e.n3 = null;
                    toLigate.push([e, elements.get(c.n3id)]);
                    //e.neighbor3 = elements.get(c.n3id);
                    //e.neighbor3.neighbor5 = e;
                    // If not, we don't set any neighbor
                }
                else {
                    e.n3 = null;
                }
            }
            // Same as above, but for 5'
            if (c.n5id >= 0) { // If we have a 5' neighbor
                let i5 = oldids.findIndex(id => { return id == c.n5id; });
                // If the 5' neighbor is also about to be added, we link to
                // the new object instead
                if (i5 >= 0) {
                    e.n5 = elems[i5];
                    elems[i5].n3 = e;
                    // Otherwise, if the indicated neighbor exists and we can link
                    // the new element to it without overwriting anything
                }
                else if (elements.has(c.n5id) &&
                    elements.get(c.n5id) &&
                    !elements.get(c.n5id).n3) {
                    e.n5 = null;
                    toLigate.push([e, elements.get(c.n5id)]);
                    // If not, we don't set any neighbor
                }
                else {
                    e.n5 = null;
                }
            }
            // Same but for basepairs
            if (c.bpid >= 0) {
                let bp = oldids.findIndex(id => { return id == c.bpid; });
                // If the paired base is also about to be added, we link to
                // the new object instead
                if (bp >= 0) {
                    e['pair'] = elems[bp];
                    elems[bp]['pair'] = e;
                    // Otherwise, if the indicated basepair exists and we can pair
                    // the new element to it without overwriting anything
                }
                else if (elements.has(c.bpid) &&
                    elements.get(c.bpid) &&
                    !elements.get(c.bpid).isPaired()) {
                    e['pair'] = elements.get(c.bpid);
                    elements.get(c.bpid)['pair'] = e;
                }
            }
        });
        // Sort out strands
        elems.forEach((e, sid) => {
            let c = instCopies[sid];
            let sys = c.system;
            // Do we have a strand assigned already?
            if (!e.strand) {
                // Does any of our neighbors know what strand this is?
                let i = e;
                while (!i.strand) { // Look in 3' dir
                    if (e === i.n3) {
                        break;
                    }
                    if (i.n3)
                        i = i.n3;
                    else
                        break;
                }
                if (!i.strand) { // If nothing, look in 5' dir
                    i = e;
                    while (!i.strand) {
                        if (e === i.n5) {
                            break;
                        }
                        if (i.n5)
                            i = i.n5;
                        else
                            break;
                    }
                }
                // If we found something
                if (i.strand) {
                    // Add us to the strand
                    e.strand = i.strand;
                    // And update endpoints
                    e.strand.updateEnds();
                }
                else {
                    // Create a new strand
                    let strand;
                    if (e.isAminoAcid()) {
                        strand = sys.addNewPeptideStrand();
                    }
                    else if (e.type == 'gs') {
                        strand = sys.addNewGenericSphereStrand();
                    }
                    else {
                        strand = sys.addNewNucleicAcidStrand();
                    }
                    e.strand = strand;
                    strand.setFrom(e);
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
            if (e.n3) {
                // Update backbone bond
                calcsp(e);
            }
            else {
                // Set explicitly to null
                e.n3 = null;
                // Remove backbone bond
                tmpSys.fillVec('bbconScales', 3, e.sid, [0, 0, 0]);
                tmpSys.bbconnector.geometry["attributes"].instanceScale.needsUpdate = true;
                render();
            }
            if (!e.n5) {
                // Set explicitly to null
                e.n5 = null;
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
    function addElementsBySeq(end, sequence, tmpSys, direction, inverse, sidCounter) {
        // add monomers to the strand
        const strand = end.strand;
        const positions = end.extendStrand(sequence.length, direction, false);
        let last = end;
        let addedElems = [];
        //create topology
        for (let i = 0, len = sequence.length; i < len; i++) {
            let e = strand.createBasicElement();
            elements.push(e); // Add element and assign id
            e.sid = sidCounter; //You're always adding to a tmpSys so this is needed
            e.dummySys = tmpSys;
            last[direction] = e;
            e[inverse] = last;
            e.type = sequence[i];
            e.strand = strand;
            last = e;
            sidCounter++;
            addedElems.push(e);
        }
        // Make last element end of strand
        last[direction] = null;
        strand.setFrom(last);
        let e = end[direction];
        //position new monomers
        for (let i = 0, len = sequence.length; i < len; i++) {
            let [p, a1, a3] = positions[i];
            e.calcPositions(p, a1, a3, true);
            e = e[direction];
        }
        addSystemToScene(tmpSys);
        //putting this in one loop would slow down loading systems
        //would require dereferencing the backbone position of every nucleotide
        //its not worth slowing down everything to avoid this for loop
        //which is much more of an edge case anyway.
        e = end;
        while (e && e[direction]) {
            // Backbone must be drawn from 5' end
            if (direction == "n5") {
                calcsp(e.n5);
            }
            else {
                calcsp(e);
            }
            e = e[direction];
        }
        return addedElems;
    }
    function addDuplexBySeq(end, sequence, tmpSys, direction, inverse, sidCounter) {
        // variables ending in "2" correspond to complement strand
        let end2 = end.findPair();
        const strand = end.strand;
        const strand2 = end2.strand;
        const l = sequence.length;
        const positions = end.extendStrand(l, direction, true); // true = double strand
        let last1 = end;
        let last2 = end2;
        let addedElems = [];
        for (let i = 0; i < l; i++) {
            let e1 = strand.createBasicElement();
            elements.push(e1);
            e1.sid = sidCounter + i;
            e1.dummySys = tmpSys;
            last1[direction] = e1;
            e1[inverse] = last1;
            e1.type = sequence[i];
            e1.strand = strand;
            last1 = e1;
            addedElems.push(e1);
        }
        for (let i = 0; i < l; i++) {
            let e1 = addedElems[i];
            let e2 = strand2.createBasicElement();
            elements.push(e2);
            e2.sid = sidCounter + l + i;
            e2.dummySys = tmpSys;
            last2[inverse] = e2;
            e2[direction] = last2;
            e2.type = e1.getComplementaryType();
            e2.strand = strand2;
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
            let [p, a1, a3] = positions[i];
            e1.calcPositions(p, a1, a3, true);
            e1 = e1[direction];
        }
        // complementary strand adds elements in reverse direction
        for (let i = l * 2 - 1; i >= l; i--) {
            let [p, a1, a3] = positions[i];
            e2.calcPositions(p, a1, a3, true);
            e2 = e2[inverse];
        }
        strand.updateEnds();
        strand2.updateEnds();
        addSystemToScene(tmpSys);
        e1 = end;
        e2 = end2;
        while (e1 && e1[direction]) {
            if (direction == "n5") {
                calcsp(e1.n5);
            }
            else {
                calcsp(e1);
            }
            e1 = e1[direction];
        }
        while (e2 && e2[inverse]) {
            if (inverse == "n5") {
                calcsp(e2.n5);
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
        if (end.n3 == null) {
            direction = "n3";
            inverse = "n5";
        }
        else if (end.n5 == null) {
            direction = "n5";
            inverse = "n3";
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
        if (end.n5 == null && end2.n3 == null) {
            direction = "n5";
            inverse = "n3";
        }
        else if (end.n3 == null && end2.n5 == null) {
            direction = "n3";
            inverse = "n5";
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
    function getSequence(elems) {
        // Sort elements in 5' to 3' order
        let strands = new Set();
        elems.forEach(e => { strands.add(e.strand); });
        let orderedElems = [];
        Array.from(strands).sort((a, b) => { return a.id < b.id ? 1 : -1; }).forEach(strand => {
            orderedElems.push(...strand.filter(e => elems.has(e)));
        });
        return orderedElems.map(e => { return e.type; }).join('');
    }
    edit.getSequence = getSequence;
    ;
    function setSequence(elems, sequence, setComplementaryBases) {
        setComplementaryBases = setComplementaryBases || false;
        if (elems.size != sequence.length) {
            notify(`You have ${elems.size} particles selected and ${sequence.length} letters in the sequence...doing my best`);
        }
        // Sort elements in 5' to 3' order
        let strands = new Set();
        elems.forEach(e => { strands.add(e.strand); });
        let orderedElems = [];
        Array.from(strands).sort((a, b) => { return a.id < b.id ? 1 : -1; }).forEach(strand => {
            orderedElems.push(...strand.filter(e => elems.has(e)));
        });
        console.log("set", orderedElems);
        // Define a function to satisfy view.longCalculation callback
        let set = function () {
            let len = Math.min(orderedElems.length, sequence.length);
            for (let i = 0; i < len; i++) {
                let e = orderedElems[i];
                e.changeType(sequence[i]);
                if (setComplementaryBases) {
                    if (e.isPaired()) {
                        let n = e;
                        n.pair.changeType(n.getComplementaryType());
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
        if (setComplementaryBases && ![...elems][0].isPaired()) {
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
        let blank = false;
        if (systems.length > 0) {
            realSys = systems.slice(-1)[0];
        }
        else {
            blank = true;
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
        elements.push(e); // Add element and assign id
        e.dummySys = tmpSys;
        e.sid = 0;
        e.type = sequence[0];
        e.n3 = null;
        e.strand = strand;
        strand.setFrom(e);
        addedElems.push(e);
        let pos, a1, a3;
        if (blank) {
            // Place new strand at origin if the scene is empty
            pos = new THREE.Vector3();
            a3 = new THREE.Vector3(0, 0, -1);
            a1 = new THREE.Vector3(0, 1, 0);
        }
        else {
            // Otherwise, place the new strand 10 units in front of the camera
            // with its a1 vector parallel to the camera heading
            // and a3 the cross product of the a1 vector and the camera's up vector
            a1 = new THREE.Vector3(0, 0, -1).applyQuaternion(camera.quaternion);
            pos = camera.position.clone().add(a1.clone().multiplyScalar(20));
            a3 = a1.clone().cross(camera.up);
        }
        e.calcPositions(pos, a1, a3, true);
        e.dummySys = tmpSys;
        // Extends the strand 3'->5' with the rest of the sequence
        // and return all added elements.
        if (createDuplex) {
            // create base pair if end doesn't have one already
            if (!e.findPair()) {
                e.pair = createBP(e);
                addedElems.push(e.pair);
            }
            addedElems = addedElems.concat(addDuplexBySeq(e, sequence.substring(1), tmpSys, "n5", "n3", 1));
        }
        else {
            addedElems = addedElems.concat(addElementsBySeq(e, sequence.substring(1), tmpSys, "n5", "n3", 1));
        }
        strand.updateEnds();
        // Make created strand(s) a new cluster, for convenience.
        clusterCounter++;
        addedElems.forEach(e => e.clusterId = clusterCounter);
        return addedElems;
    }
    edit.createStrand = createStrand;
    /**
     * Experimental Function that Discretizes Mass of system (every particle has same mass currently 1)
     */
    function discretizeMass(elems, cellsize) {
        //cellsize must be in Angstroms as we convert it here
        cellsize /= 8.518; //Angstrom to sim unit length
        // get positions from Three Vector returned from getPos()
        let xPositions = elems.map(e => e.getPos().x);
        let yPositions = elems.map(e => e.getPos().y);
        let zPositions = elems.map(e => e.getPos().z);
        let xmax = 0, xmin = 0;
        let ymax = 0, ymin = 0;
        let zmax = 0, zmin = 0;
        // Useful for building 3d Grid
        xmax = Math.max(...xPositions);
        xmin = Math.min(...xPositions);
        ymax = Math.max(...yPositions);
        ymin = Math.min(...yPositions);
        zmax = Math.max(...zPositions);
        zmin = Math.min(...zPositions);
        // Assign boxids based off position in the grid
        let xboxids = [];
        let yboxids = [];
        let zboxids = [];
        let elemids = [];
        let elemIndx = 0;
        elems.forEach(elem => {
            let xbid = Math.floor((elem.getPos().x - xmin) / cellsize);
            let ybid = Math.floor((elem.getPos().y - ymin) / cellsize);
            let zbid = Math.floor((elem.getPos().z - zmin) / cellsize);
            xboxids.push(xbid);
            yboxids.push(ybid);
            zboxids.push(zbid);
            elemids.push(elemIndx);
            elemIndx += 1;
        });
        // Numbers for looping through boxids
        let xGridNum = Math.ceil((xmax - xmin) / cellsize);
        let yGridNum = Math.ceil((ymax - ymin) / cellsize);
        let zGridNum = Math.ceil((zmax - zmin) / cellsize);
        // New Particle Arrays
        let gPositions = [];
        let gMasses = [];
        let indexfile = [];
        let currentBox = [];
        //Sort through boxids and generate array for intialization of new particles
        for (let i = 0; i < xGridNum; i += 1) {
            for (let j = 0; j < yGridNum; j += 1) {
                for (let k = 0; k < zGridNum; k += 1) {
                    //returns only boxid entries matching the current i, j, k
                    currentBox = [];
                    for (let p = 0; p < xboxids.length; p++) {
                        if (xboxids[p] == i && yboxids[p] == j && zboxids[p] == k) {
                            currentBox.push(elemids[p]);
                        }
                    }
                    //If any particle in this section of the grid
                    if (currentBox.length > 0) {
                        indexfile.push(currentBox);
                        let m = currentBox.length;
                        let com = new THREE.Vector3(0, 0, 0);
                        for (let l = 0; l < m; l += 1) {
                            com.add(elems[currentBox[l]].getPos());
                        }
                        com = com.divideScalar(m);
                        gPositions.push(com.clone());
                        gMasses.push(m);
                    }
                }
            }
        }
        // Now I need to Return the New System
        //dummy system
        let dumb = new System(tmpSystems.length, 0);
        dumb.initInstances(gPositions.length);
        tmpSystems.push(dumb);
        let currentelemsize = elements.size;
        let realSys = new System(sysCount++, currentelemsize);
        realSys.initInstances(0);
        systems.push(realSys);
        addSystemToScene(realSys);
        let gstrand = realSys.addNewGenericSphereStrand();
        let newElems = [];
        let last;
        for (let i = 0; i < gPositions.length; i++) {
            let be = gstrand.createBasicElement(currentelemsize + i);
            elements.push(be);
            be.sid = i;
            be.dummySys = dumb;
            be.type = 'gs';
            be.n5 = null;
            if (i != 0) {
                let prev = newElems[i - 1];
                be.n3 = prev;
                prev.n5 = be;
            }
            else {
                be.n3 = null;
            }
            be.strand = gstrand;
            be.mass = gMasses[i];
            newElems.push(be);
            last = be;
        }
        gstrand.setFrom(last);
        newElems.forEach((e, eid) => {
            e.calcPositions(gPositions[eid]);
        });
        addSystemToScene(dumb); // add tmpSys to scene
        return { elems: newElems, indx: indexfile };
    }
    edit.discretizeMass = discretizeMass;
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
        const strand = elem.getSystem().addNewNucleicAcidStrand();
        // Add element and assign id
        const e = new DNANucleotide(undefined, strand);
        elements.push(e);
        e.dummySys = tmpSys;
        e.sid = 0;
        e.type = elem.getComplementaryType();
        e.n3 = null;
        e.n5 = null;
        e.pair = elem;
        elem.pair = e;
        e.strand = strand;
        strand.setFrom(e);
        const cm = elem.getPos();
        const a1 = elem.getA1();
        const a3 = elem.getA3();
        // calculate position of base pair
        a1.negate();
        a3.negate();
        const pos = cm.clone().sub(a1.clone().multiplyScalar(1.2));
        e.calcPositions(pos, a1, a3, true);
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
