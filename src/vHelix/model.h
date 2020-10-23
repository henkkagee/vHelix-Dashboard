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

void rotate_vector_by_quaternion(const QVector3D &v, const QQuaternion& q, QVector3D& vprime);

namespace Model {

    struct Helix;
    struct Strand;
    class Base;

    class Base {
    public:

        Base(const QVector3D &pos, Type type, int offset);
        Base(long long int strandIdD, unsigned char &nb, long long int base3neigh, long long int base5neigh, unsigned long long int baseId);

        void connect_forward(Base & target, bool ignorePreviousConnections = false);
        void disconnect_forward(bool ignorePerpendicular = false);
        void disconnect_backward(bool ignorePerpendicular = false);
        void connect_opposite(Base & target, bool ignorePreviousConnections = false);
        void disconnect_opposite();

        void setForward(Base *base);
        void setBackward(Base *base);
        void setOpposite(Base *base);
        void setBase(Nucleobase nb);

        Base *getForward();
        Base *getBackward();
        Base *getOpposite();
        Helix *getParent();

        // member variables
        QVector3D position;
        Base *forward;
        Base *backward;
        Base *opposite;
        Helix *parent;
        Type type;
        Nucleobase nucleobase;
        bool in_strand;
        int offset_;
        int checked;

        std::string strandname_;

        long long int base3neigh_;
        long long int base5neigh_;
        long long int strandId_;
        unsigned long long int baseId_;

        // only for strand traversal, not actual connections
        Base *strand_forward;
        Base *strand_backward;

    };

    struct Helix {
        Base *getBackwardFivePrime();
        Base *getBackwardThreePrime();
        Base *getForwardFivePrime();
        Base *getForwardThreePrime();

        Helix(const QVector3D & position, const QQuaternion & orientation, const char *name, unsigned int bases = 0);
        Helix() : bases_(0) {}
        bool operator==(const char *name) const { return this->name_ == name; }

        QVector3D position_;
        QQuaternion orientation_;
        std::string name_;
        int bases_;
        bool sequence_assigned_;

        // strands forward and backward
        // probably not the most convenient data structure for later editing but it's fine in this context for now
        // must reserve the space for all bases immediately in order to avoid reallocation and invalidation of element pointers
        std::vector<Base> Fbases;
        std::vector<Base> Bbases;
        Base *backward5Prime;
        Base *backward3Prime;
        Base *forward5Prime;
        Base *forward3Prime;

        QColor color;
    };

    struct Strand {
        Strand() : sequenced_(false) {}
        Strand(unsigned long long int length, unsigned long long int id) : length_(length), id_(id), scaffold_(false) {}

        std::vector<Base*> bases_;
        bool sequenced_;
        int length_;

        // use either one
        std::string name_;
        long long int id_;

        bool scaffold_;
        bool forward_;
    };

}

#endif // MODEL_H
