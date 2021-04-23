class EditHistory {
    constructor() {
        this.undoStack = new Stack();
        this.redoStack = new Stack();
    }
    ;
    /**
     * Performs revertable edit and add it to the undo history stack
     * @param edit object describing the revertable edit.
     */
    do(edit) {
        edit.do();
        this.undoStack.push(edit);
        // We no longer care about the alternate future:
        this.redoStack = new Stack();
        // Update the hierarchy, since we've made changes
        // if it is open, otherwise we don't care !
        if (view.isWindowOpen('systemHierarchyWindow'))
            drawSystemHierarchy();
        //Return focus to the canvas so undo can be called immediatley
        canvas.focus();
    }
    /**
     * Add revertable edit to the undo history stack without performing it.
     * @param edit object describing the revertable edit.
     */
    add(edit) {
        this.undoStack.push(edit);
        // Update the hierarchy, since we've made changes
        if (view.isWindowOpen('systemHierarchyWindow'))
            drawSystemHierarchy();
    }
    undo() {
        let edit;
        try {
            edit = this.undoStack.pop();
        }
        catch (e) {
            return; // Cannot undo any further
        }
        edit.undo();
        this.redoStack.push(edit);
        // Update the hierarchy, since we've made changes
        if (view.isWindowOpen('systemHierarchyWindow'))
            drawSystemHierarchy();
    }
    redo() {
        let edit;
        try {
            edit = this.redoStack.pop();
        }
        catch (e) {
            return; // Cannot undo any further
        }
        edit.do();
        this.undoStack.push(edit);
        // Update the hierarchy, since we've made changes
        if (view.isWindowOpen('systemHierarchyWindow'))
            drawSystemHierarchy();
    }
}
class RevertableEdit {
    /**
     * Takes undo and redo functions as arguments.
     * @param undo function that, when applied, will revert the edit
     * @param redo function that, when applied, will perform the edit
     */
    constructor(undoFunction, doFunction) {
        this.undo = undoFunction;
        this.do = doFunction;
    }
    ;
}
class RevertableMultiEdit extends RevertableEdit {
    constructor(edits) {
        super(() => { edits.slice().reverse().forEach(edit => { edit.undo(); }); }, () => { edits.forEach(edit => { edit.do(); }); });
    }
}
class RevertableAddition extends RevertableEdit {
    constructor(saved, added, pos) {
        const s = saved;
        let a = added;
        const p = pos;
        let undo = function () { edit.deleteElements(a); };
        let redo = function () { a = edit.addElementsAt(s, p); };
        super(undo, redo);
    }
    ;
}
class RevertableDeletion extends RevertableEdit {
    constructor(victims) {
        const saved = victims.map(e => new InstanceCopy(e));
        let undo = function () { this.victims = edit.addElements(saved); };
        let redo = function () { edit.deleteElements(this.victims); };
        super(undo, redo);
        this.victims = victims;
    }
    ;
}
class RevertableNick extends RevertableEdit {
    constructor(element) {
        const end3 = element.id;
        const end5 = element.n3.id;
        let undo = function () { edit.ligate(elements.get(end3), elements.get(end5)); };
        let redo = function () { edit.nick(elements.get(end3)); };
        super(undo, redo);
    }
    ;
}
class RevertableLigation extends RevertableEdit {
    constructor(e1, e2) {
        let end5, end3;
        // Find out which is the 5' end and which is 3'
        if (!e1.n5 && !e2.n3) {
            end5 = e1.id;
            end3 = e2.id;
        }
        else if (!e1.n3 && !e2.n5) {
            end5 = e2.id;
            end3 = e1.id;
        }
        else {
            notify("Please select one nucleotide with an available 3' connection and one with an available 5'");
            super(() => { }, () => { });
            return;
        }
        let undo = function () { edit.nick(elements.get(end3)); };
        let redo = function () { edit.ligate(elements.get(end5), elements.get(end3)); };
        super(undo, redo);
    }
    ;
}
class RevertableSequenceEdit extends RevertableEdit {
    constructor(elems, sequence, setComplementaryBases) {
        const oldseq = edit.getSequence(elems);
        let undo = function () { edit.setSequence(elems, oldseq, setComplementaryBases); };
        let redo = function () { edit.setSequence(elems, sequence, setComplementaryBases); };
        super(undo, redo);
    }
    ;
}
class RevertableTranslation extends RevertableEdit {
    constructor(translatedElements, translationVector) {
        const elements = new Set(translatedElements);
        const v = translationVector.clone();
        let undo = function () {
            translateElements(elements, v.clone().negate());
        };
        let redo = function () {
            translateElements(elements, v);
        };
        super(undo, redo);
    }
    ;
}
class RevertableRotation extends RevertableEdit {
    constructor(rotatedElements, axis, angle, about) {
        const elements = new Set(rotatedElements);
        const c = about.clone();
        let undo = function () {
            rotateElements(elements, axis, -1 * angle, c);
        };
        let redo = function () {
            rotateElements(elements, axis, angle, c);
        };
        super(undo, redo);
    }
    ;
}
class RevertableTransformation extends RevertableEdit {
    constructor(transformedElements, translation, rotation, about) {
        const elements = new Set(transformedElements);
        const c = about.clone();
        const t = translation.clone();
        const r = rotation.clone();
        let undo = function () {
            rotateElementsByQuaternion(elements, r.clone().conjugate(), c);
            translateElements(elements, t.clone().negate());
            if (selectedBases.size > 0 && view.transformMode.enabled()) {
                transformControls.show();
            }
            else {
                transformControls.hide();
            }
        };
        let redo = function () {
            translateElements(elements, t);
            rotateElementsByQuaternion(elements, r, c);
            if (selectedBases.size > 0 && view.transformMode.enabled()) {
                transformControls.show();
            }
            else {
                transformControls.hide();
            }
        };
        super(undo, redo);
    }
    ;
}
class RevertableClusterSim extends RevertableEdit {
    constructor(clusters) {
        let cs = [];
        clusters.forEach((c) => {
            cs.push({
                "elems": c.getElements(),
                "transl": c.getTotalTranslation(),
                "rot": c.getTotalRotation(),
                "pos": c.getPosition(),
            });
        });
        let undo = function () {
            cs.forEach((c) => {
                rotateElementsByQuaternion(c["elems"], c["rot"].clone().conjugate(), c["pos"]);
                translateElements(c["elems"], c["transl"].clone().negate());
            });
        };
        let redo = function () {
            cs.forEach((c) => {
                translateElements(c["elems"], c["transl"]);
                rotateElementsByQuaternion(c["elems"], c["rot"], c["pos"]);
            });
        };
        super(undo, redo);
    }
    ;
}
// Adapted from https://github.com/worsnupd/ts-data-structures
class Stack {
    constructor() {
        this.size = 0;
    }
    push(data) {
        this.top = new StackElem(data, this.top);
        this.size++;
    }
    pop() {
        if (this.isEmpty()) {
            throw new Error('Empty stack');
        }
        const data = this.top.data;
        this.top = this.top.next;
        this.size--;
        return data;
    }
    peek() {
        if (this.isEmpty()) {
            throw new Error('Empty stack');
        }
        return this.top.data;
    }
    isEmpty() {
        return this.size === 0;
    }
}
class StackElem {
    constructor(data, next) {
        this.data = data;
        this.next = next;
        ;
    }
}
