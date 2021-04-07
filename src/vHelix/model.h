/*
 *  Copyright (c) 2020 Henrik Granö
 *  See the license in the root directory for the full notice
 *
 * Base, Helix and Strand data classes.
*/

#ifndef MODEL_H
#define MODEL_H
#include <string>
#include <iostream>
#include <QVector3D>
#include <QQuaternion>
#include <QColor>
#include <array>

#include "dna.h"
#include "utils.h"

enum Nucleobase {
    ADENINE = 0,
    CYTOSINE = 1,
    GUANINE = 2,
    THYMINE = 3,
    NONE = 4
};

enum Type {
    BASE = 0,
    FIVE_PRIME_END = 1,
    THREE_PRIME_END = 2,
    END = 3
};



namespace Model {

    struct Helix;
    struct Strand;
    class Base;

    class Base {
    public:

        Base(const QVector3D &pos, Type type, int v_offset, unsigned long BaseId);
        Base(unsigned int strandIdD, std::string &nb, long base3neigh, long base5neigh, unsigned long BaseId, long oppneigh);

        void setForward(Base *base);
        void setBackward(Base *base);
        void setOpposite(Base *base);
        void setParent(Helix *helix);
        void setBase(Nucleobase nb);
        void setType(Type type);
        void setPos(QVector3D pos);

        Base *getForward();
        Base *getBackward();
        Base *getOpposite();
        Helix *getParent();
        Type &getType();
        Nucleobase &getBase();
        QVector3D &getPos();

        // member variables
        bool in_strand_;
        int offset_;
        bool checked_;
        QVector3D a3_;

        // ID's
        // existing base ID's are always positive integers, but forward/backward/neighbouring ID's can be negative
        // to indicate that no such neighbour exists
        long base3neigh_;
        long base5neigh_;
        long oppneigh_;
        unsigned int strandId_;
        long baseId_;

        // for finding the neighbouring strands in the structure that are not connected
        Base *strand_forward_;
        Base *strand_backward_;

        Base *forward_;

    private:
        QVector3D position_;
        Base *backward_;
        Base *opposite_;
        Helix *parent_;
        Type type_;
        Nucleobase nucleobase_;
    };

    struct Helix {
        Base *getBackwardFivePrime();
        Base *getBackwardThreePrime();
        Base *getForwardFivePrime();
        Base *getForwardThreePrime();

        // bIdStart = the first available base id to start assigning to new bases in the constructor
        Helix(const QVector3D & position, const QQuaternion & orientation, const char *name, unsigned long bIdStart, unsigned int bases = 0);
        Helix() : bases_(0) {}
        bool operator==(const char *name) const { return this->name_ == name; }

        QVector3D position_;
        QQuaternion orientation_;
        std::string name_;
        int bases_;
        bool sequence_assigned_;

        // strands forward and backward
        // probably not the most convenient data structure for later editing but it's efficient
        // must reserve the space for all bases immediately in order to avoid reallocation and invalidation of element pointers
        std::vector<Base> Fbases_;
        std::vector<Base> Bbases_;
        Base *backward5Prime_;
        Base *backward3Prime_;
        Base *forward5Prime_;
        Base *forward3Prime_;

        QColor color;
    };

    struct Strand {
        Strand() : sequenced_(false) {}
        Strand(unsigned long long int length, unsigned long id) : length_(length), id_(id), scaffold_(false), sequenced_(false) {}

        std::vector<Base*> bases_;
        bool sequenced_;
        int length_;

        unsigned long id_;

        bool scaffold_;
        bool forward_;
    };

}

#endif // MODEL_H
