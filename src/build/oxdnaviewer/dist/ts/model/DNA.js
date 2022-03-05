/**
 * Extends Nuculeotide with DNA-specific properties such as base position relative to backbone, and B-form helix creation
 */
class DNANucleotide extends Nucleotide {
    constructor(id, strand) {
        super(id, strand);
        this.elementType = DNA;
        this.bbnsDist = 0.8147053;
    }
    ;
    calcBBPos(p, a1, a2, a3) {
        return new THREE.Vector3(p.x - (0.34 * a1.x + 0.3408 * a2.x), p.y - (0.34 * a1.y + 0.3408 * a2.y), p.z - (0.34 * a1.z + 0.3408 * a2.z));
    }
    ;
    getA2() {
        const cm = this.getPos();
        const bb = this.getInstanceParameter3("bbOffsets");
        const a1 = this.getA1();
        return bb.clone().sub(cm).add(a1.clone().multiplyScalar(0.34)).divideScalar(-0.3408);
    }
    getA3() {
        const a1 = this.getA1();
        const a2 = this.getA2();
        const a3 = a1.clone().cross(a2).divideScalar(-a1.dot(a1));
        return a3;
    }
    ;
    // Uses method from generate-sa.py.  Needs to be relaxed since this is oxDNA1 helix
    extendStrand(len, direction, double) {
        const rot = 35.9 * Math.PI / 180; // 0.68940505
        let rise = 0.3897628551303122;
        const startPos = this.getPos();
        const oldA1 = this.getA1();
        let dir = this.getA3();
        // normalize dir
        const dir_norm = Math.sqrt(dir.clone().dot(dir));
        dir.divideScalar(dir_norm);
        const a1 = oldA1.clone();
        const center = startPos.add(a1.clone().multiplyScalar(0.6));
        // create rotational matrix
        let R = new THREE.Matrix4;
        if (direction == "n3") {
            R.makeRotationAxis(dir.clone().negate(), rot);
            rise = -rise;
        }
        else { // n5
            R.makeRotationAxis(dir, rot);
        }
        let rb = new THREE.Vector3(0, 0, 0);
        const a3 = dir;
        const rbShift = a3.clone().multiplyScalar(rise);
        const out = [];
        // add single strand
        for (let i = 0; i < len; i++) {
            a1.applyMatrix4(R);
            rb.add(rbShift);
            let p = rb.clone().add(center).sub(a1.clone().multiplyScalar(0.6));
            out.push([p, a1.clone(), a3.clone()]);
        }
        // add complementary strand in the opposite direction
        if (double) {
            a1.negate();
            a3.negate();
            rbShift.negate();
            R.transpose();
            let p1 = rb.clone().add(center).sub(a1.clone().multiplyScalar(0.6));
            out.push([p1, a1.clone(), a3.clone()]);
            for (let i = 0; i < len - 1; i++) {
                a1.applyMatrix4(R);
                rb.add(rbShift);
                let p2 = rb.clone().add(center).sub(a1.clone().multiplyScalar(0.6));
                out.push([p2, a1.clone(), a3.clone()]);
            }
        }
        return out;
    }
    getComplementaryType() {
        var map = { A: 'T', G: 'C', C: 'G', T: 'A' };
        return map[this.type];
    }
    toJSON() {
        // Get superclass attributes
        let json = super.toJSON();
        json['class'] = 'DNA';
        return json;
    }
}
;
