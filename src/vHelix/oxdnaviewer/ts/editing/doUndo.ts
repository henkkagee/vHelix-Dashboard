
class EditHistory {
    private undoStack: Stack<RevertableEdit>;
    private redoStack: Stack<RevertableEdit>;

    constructor() {
        this.undoStack = new Stack<RevertableEdit>();
        this.redoStack = new Stack<RevertableEdit>();
    };

    /**
     * Performs revertable edit and add it to the undo history stack
     * @param edit object describing the revertable edit.
     */
    public do(edit: RevertableEdit) {
        edit.do();
        this.undoStack.push(edit);

        // We no longer care about the alternate future:
        this.redoStack = new Stack<RevertableEdit>();

        // Update the hierarchy, since we've made changes
        // if it is open, otherwise we don't care !
        if(view.isWindowOpen('systemHierarchyWindow'))
            drawSystemHierarchy();

        //Return focus to the canvas so undo can be called immediatley
        canvas.focus()
    }

    /**
     * Add revertable edit to the undo history stack without performing it.
     * @param edit object describing the revertable edit.
     */
    public add(edit: RevertableEdit) {
        this.undoStack.push(edit);

        // Update the hierarchy, since we've made changes
        if(view.isWindowOpen('systemHierarchyWindow'))
            drawSystemHierarchy();
    }

    public undo() {
        let edit: RevertableEdit;
        try {
            edit = this.undoStack.pop();
        } catch(e) {
            return // Cannot undo any further
        }
        edit.undo();
        this.redoStack.push(edit);

        // Update the hierarchy, since we've made changes
        if(view.isWindowOpen('systemHierarchyWindow'))
            drawSystemHierarchy();
    }

    public redo() {
        let edit: RevertableEdit;
        try {
            edit = this.redoStack.pop();
        } catch(e) {
            return // Cannot undo any further
        }
        edit.do();
        this.undoStack.push(edit);

        // Update the hierarchy, since we've made changes
        if(view.isWindowOpen('systemHierarchyWindow'))
            drawSystemHierarchy();
    }
}

class RevertableEdit {
    undo: () => void;
    do: () => void;

    /**
     * Takes undo and redo functions as arguments.
     * @param undo function that, when applied, will revert the edit
     * @param redo function that, when applied, will perform the edit
     */
    constructor(undoFunction: () => void, doFunction: () => void) {
        this.undo = undoFunction;
        this.do = doFunction;
    };
}

class RevertableMultiEdit extends RevertableEdit {
    constructor(edits:RevertableEdit[]) {
        super(
            ()=>{edits.slice().reverse().forEach(edit=>{edit.undo()})},
            ()=>{edits.forEach(edit=>{edit.do()})}
        );
    }
}

class RevertableAddition extends RevertableEdit {
    constructor(saved:InstanceCopy[], added: BasicElement[], pos?: THREE.Vector3) {
        const s = saved;
        let a = added;
        const p = pos;

        let undo = function() {edit.deleteElements(a)};
        let redo = function() {a = edit.addElementsAt(s, p)};
        super(undo, redo);
    };
}

class RevertableDeletion extends RevertableEdit {
    victims: BasicElement[];
    constructor(victims: BasicElement[]) {
        const saved = victims.map(e=> new InstanceCopy(e));
        let undo = function() {this.victims = edit.addElements(saved)};
        let redo = function() {edit.deleteElements(this.victims)};
        super(undo, redo);
        this.victims = victims;
    };
}

class RevertableNick extends RevertableEdit {
    constructor(element: BasicElement) {
        const end3 = element.gid;
        const end5 = element.neighbor3.gid;
        let undo = function() {edit.ligate(elements.get(end3), elements.get(end5))};
        let redo = function() {edit.nick(elements.get(end3));}
        super(undo, redo);
    };
}

class RevertableLigation extends RevertableEdit {
    constructor(e1 :BasicElement, e2: BasicElement) {
        let end5: number, end3: number;
        // Find out which is the 5' end and which is 3'
        if (!e1.neighbor5 && !e2.neighbor3) {
            end5 = e1.gid;
            end3 = e2.gid;
        } else if (!e1.neighbor3 && !e2.neighbor5) {
            end5 = e2.gid;
            end3 = e1.gid;
        } else {
            notify("Please select one nucleotide with an available 3' connection and one with an available 5'");
            super(()=>{}, ()=>{});
            return;
        }
        let undo = function() {edit.nick(elements.get(end3))};
        let redo = function() {edit.ligate(elements.get(end5), elements.get(end3))}
        super(undo, redo);
    };
}

class RevertableSequenceEdit extends RevertableEdit {
    constructor(elems: Nucleotide[], sequence: string, setComplementaryBases?: boolean) {
        const oldseq = api.getSequence(elems);
        let undo = function() {edit.setSequence(elems, oldseq, setComplementaryBases)};
        let redo = function() {edit.setSequence(elems, sequence, setComplementaryBases)}
        super(undo, redo);
    };
}

class RevertableTranslation extends RevertableEdit {
    constructor(translatedElements: Set<BasicElement>,translationVector: THREE.Vector3) {
        const elements = new Set(translatedElements);
        const v = translationVector.clone();
        let undo = function() {
            translateElements(elements, v.clone().negate());
        };

        let redo = function() {
            translateElements(elements, v);
        }

        super(undo, redo);
    };
}

class RevertableRotation extends RevertableEdit {
    constructor(rotatedElements: Set<BasicElement>, axis: THREE.Vector3, angle: number, about: THREE.Vector3) {
        const elements = new Set(rotatedElements);
        const c = about.clone();
        let undo = function() {
            rotateElements(elements, axis, -1*angle, c);
        };

        let redo = function() {
            rotateElements(elements, axis, angle, c);
        }

        super(undo, redo);
    };
}

class RevertableTransformation extends RevertableEdit {
    constructor(
        transformedElements: Set<BasicElement>,
        translation: THREE.Vector3,
        rotation: THREE.Quaternion,
        about: THREE.Vector3) 
        {
        const elements = new Set(transformedElements);
        const c = about.clone();
        const t = translation.clone();
        const r = rotation.clone();
        let undo = function() {
            rotateElementsByQuaternion(elements, r.clone().conjugate(), c);
            translateElements(elements, t.clone().negate());
            if (selectedBases.size > 0 && view.transformMode.enabled()) {
                transformControls.show();
            } else {
                transformControls.hide();
            }
        };

        let redo = function() {
            translateElements(elements, t);
            rotateElementsByQuaternion(elements, r, c);
            if (selectedBases.size > 0 && view.transformMode.enabled()) {
                transformControls.show();
            } else {
                transformControls.hide();
            }
        }

        super(undo, redo);
    };
}

class RevertableClusterSim extends RevertableEdit {
    constructor(clusters: Cluster[]) {
        let cs = [];
        clusters.forEach((c) => {
            cs.push({
                "elems": c.getElements(),
                "transl": c.getTotalTranslation(),
                "rot": c.getTotalRotation(),
                "pos": c.getPosition(),
            });
        });
        let undo = function() {
            cs.forEach((c) => {
                rotateElementsByQuaternion(
                    c["elems"], c["rot"].clone().conjugate(), c["pos"]
                );
                translateElements(
                    c["elems"], c["transl"].clone().negate()
                );
            });
        };
        let redo = function() {
            cs.forEach((c) => {
                translateElements(
                    c["elems"], c["transl"]
                );
                rotateElementsByQuaternion(
                    c["elems"], c["rot"], c["pos"]
                );
            });
        }
        super(undo, redo);
    };
}

// Adapted from https://github.com/worsnupd/ts-data-structures
class Stack<T> {
    private top: StackElem<T>;
    public size: number = 0;

    public push(data: T): void {
        this.top = new StackElem(data, this.top);
        this.size++;
    }

    public pop(): T {
        if (this.isEmpty()) {
            throw new Error('Empty stack');
        }
        const data = this.top.data;
        this.top = this.top.next;
        this.size--;

        return data;
    }

    public peek(): T {
        if (this.isEmpty()) {
            throw new Error('Empty stack');
        }
        return this.top.data;
    }

    public isEmpty(): boolean {
        return this.size === 0;
    }
}

class StackElem<T> {
    constructor(
        public data: T,
        public next: StackElem<T>,
    ) {;}
}