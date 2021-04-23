//THREE quaternions are in (x, y, z, w) order
//GLSL quaternions are in (w, z, y, x) order
//So when you need to convert between them...
function glsl2three(input) {
    let out = new THREE.Quaternion(input.w, input.z, input.y, input.x);
    return out;
}
function rotateElements(elements, axis, angle, about) {
    let q = new THREE.Quaternion();
    q.setFromAxisAngle(axis, angle);
    rotateElementsByQuaternion(elements, q, about);
    if (forceHandler)
        forceHandler.redraw();
}
function rotateElementsByQuaternion(elements, q, about) {
    // For some reason, we have to rotate the orientations
    // around an axis with inverted y-value...
    let q2 = q.clone();
    q2.y *= -1;
    elements.forEach((e) => {
        let sys = e.getSystem();
        let sid = e.sid;
        if (e.dummySys !== null) {
            sys = e.dummySys;
        }
        //get current positions
        let cmPos = e.getPos();
        let bbPos = e.getInstanceParameter3("bbOffsets");
        let nsPos = e.getInstanceParameter3("nsOffsets");
        let conPos = e.getInstanceParameter3("conOffsets");
        let bbconPos = e.getInstanceParameter3("bbconOffsets");
        //the rotation center needs to be (0,0,0)
        cmPos.sub(about);
        bbPos.sub(about);
        nsPos.sub(about);
        conPos.sub(about);
        bbconPos.sub(about);
        cmPos.applyQuaternion(q);
        bbPos.applyQuaternion(q);
        nsPos.applyQuaternion(q);
        conPos.applyQuaternion(q);
        bbconPos.applyQuaternion(q);
        //get current rotations and convert to THREE coordinates
        let nsRotationV = e.getInstanceParameter4("nsRotation");
        let nsRotation = glsl2three(nsRotationV);
        let conRotationV = e.getInstanceParameter4("conRotation");
        let conRotation = glsl2three(conRotationV);
        let bbconRotationV = e.getInstanceParameter4("bbconRotation");
        let bbconRotation = glsl2three(bbconRotationV);
        //apply individual object rotation
        nsRotation.multiply(q2);
        conRotation.multiply(q2);
        bbconRotation.multiply(q2);
        //move the object back to its original position
        cmPos.add(about);
        bbPos.add(about);
        nsPos.add(about);
        conPos.add(about);
        bbconPos.add(about);
        //update the instancing matrices
        sys.fillVec('cmOffsets', 3, sid, [cmPos.x, cmPos.y, cmPos.z]);
        sys.fillVec('bbOffsets', 3, sid, [bbPos.x, bbPos.y, bbPos.z]);
        sys.fillVec('nsOffsets', 3, sid, [nsPos.x, nsPos.y, nsPos.z]);
        sys.fillVec('conOffsets', 3, sid, [conPos.x, conPos.y, conPos.z]);
        sys.fillVec('bbconOffsets', 3, sid, [bbconPos.x, bbconPos.y, bbconPos.z]);
        sys.fillVec('nsRotation', 4, sid, [nsRotation.w, nsRotation.z, nsRotation.y, nsRotation.x]);
        sys.fillVec('conRotation', 4, sid, [conRotation.w, conRotation.z, conRotation.y, conRotation.x]);
        sys.fillVec('bbconRotation', 4, sid, [bbconRotation.w, bbconRotation.z, bbconRotation.y, bbconRotation.x]);
    });
    // Update backbone connections for bases with neigbours outside the selection set
    elements.forEach((base) => {
        if (base.n3 !== null && base.n3 !== undefined && !elements.has(base.n3)) {
            calcsp(base); //calculate sp between current and n3
        }
        if (base.n5 !== null && base.n5 !== undefined && !elements.has(base.n5)) {
            calcsp(base.n5); //calculate sp between current and n5
        }
    });
    for (let i = 0; i < systems.length; i++) {
        systems[i].callUpdates(['instanceOffset', 'instanceRotation']);
    }
    for (let i = 0; i < tmpSystems.length; i++) {
        tmpSystems[i].callUpdates(['instanceOffset', 'instanceRotation']);
    }
    render();
}
//adjust the backbone after the move. Copied from DragControls
function calcsp(currentNuc) {
    let sys = currentNuc.getSystem();
    if (currentNuc.dummySys !== null) {
        sys = currentNuc.dummySys;
    }
    let temp;
    try {
        temp = currentNuc.n3.getInstanceParameter3("bbOffsets");
    }
    catch (error) {
        notify("Can't calculate backbone connection for particle " + currentNuc.id + " because there is no upstream connection");
        return;
    }
    let xbbLast = temp.x, ybbLast = temp.y, zbbLast = temp.z;
    temp = currentNuc.getInstanceParameter3("bbOffsets"); //get currentNuc's backbone world position
    let xbb = temp.x;
    let ybb = temp.y;
    let zbb = temp.z;
    //calculate sp location, length and orientation
    let xsp = (xbb + xbbLast) / 2, ysp = (ybb + ybbLast) / 2, zsp = (zbb + zbbLast) / 2;
    let spLen = Math.sqrt(Math.pow(xbb - xbbLast, 2) + Math.pow(ybb - ybbLast, 2) + Math.pow(zbb - zbbLast, 2));
    let spRotation = new THREE.Quaternion().setFromUnitVectors(new THREE.Vector3(0, 1, 0), new THREE.Vector3(xsp - xbb, ysp - ybb, zsp - zbb).normalize());
    currentNuc.setInstanceParameter('bbconOffsets', [xsp, ysp, zsp]);
    currentNuc.setInstanceParameter('bbconRotation', [spRotation.w, spRotation.z, spRotation.y, spRotation.x]);
    //introduce distance based cutoff of the backbone connectors
    if (spLen >= box.x * .9 || spLen >= box.y * .9 || spLen >= box.z * .9 || currentNuc.isGS()) {
        currentNuc.setInstanceParameter('bbconScales', [0, 0, 0]);
    }
    else {
        currentNuc.setInstanceParameter('bbconScales', [1, spLen, 1]);
    }
    sys.bbconnector.geometry["attributes"].instanceOffset.needsUpdate = true;
    sys.bbconnector.geometry["attributes"].instanceRotation.needsUpdate = true;
    sys.bbconnector.geometry["attributes"].instanceScale.needsUpdate = true;
}
function translateElements(elements, v) {
    elements.forEach((e) => {
        let sys = e.getSystem();
        let sid = e.sid;
        if (e.dummySys !== null) {
            sys = e.dummySys;
        }
        let cmPos = e.getPos();
        let bbPos = e.getInstanceParameter3("bbOffsets");
        let nsPos = e.getInstanceParameter3("nsOffsets");
        let conPos = e.getInstanceParameter3("conOffsets");
        let bbconPos = e.getInstanceParameter3("bbconOffsets");
        cmPos.add(v);
        bbPos.add(v);
        nsPos.add(v);
        conPos.add(v);
        bbconPos.add(v);
        sys.fillVec('cmOffsets', 3, sid, [cmPos.x, cmPos.y, cmPos.z]);
        sys.fillVec('bbOffsets', 3, sid, [bbPos.x, bbPos.y, bbPos.z]);
        sys.fillVec('nsOffsets', 3, sid, [nsPos.x, nsPos.y, nsPos.z]);
        sys.fillVec('conOffsets', 3, sid, [conPos.x, conPos.y, conPos.z]);
        sys.fillVec('bbconOffsets', 3, sid, [bbconPos.x, bbconPos.y, bbconPos.z]);
    });
    // Update backbone connections (is there a more clever way to do this than
    // to loop through all? We only need to update bases with neigbours
    // outside the selection set)
    elements.forEach((base) => {
        if (base.n3 !== null && base.n3 !== undefined) {
            calcsp(base); //calculate sp between current and n3
        }
        if (base.n5 !== null && base.n5 !== undefined) {
            calcsp(base.n5); //calculate sp between current and n5
        }
    });
    for (let i = 0; i < systems.length; i++) {
        systems[i].callUpdates(['instanceOffset']);
    }
    for (let i = 0; i < tmpSystems.length; i++) {
        tmpSystems[i].callUpdates(['instanceOffset']);
    }
    if (forceHandler)
        forceHandler.redraw();
    render();
}
//dragControls.activate();
//dragControls.enabled = true;
