#pragma once
#include "Serialize_Buffer.h"

struct PACKET_HEADER {
    unsigned char h_code;
    unsigned char h_size;
    unsigned char h_type;

    void Serialize(CMessage& buffer) {
        buffer << h_code;
        buffer << h_size;
        buffer << h_type;
    }

    void Deserialize(CMessage& buffer) {
        buffer >> h_code;
        buffer >> h_size;
        buffer >> h_type;
    }
};

struct SC_CREATE_CHARACTER {
    int id;
    char dir;
    short x;
    short y;
    char hp;

    void Serialize(CMessage& buffer) {
        buffer << id;
        buffer << dir;
        buffer << x;
        buffer << y;
        buffer << hp;
    }

    void Deserialize(CMessage& buffer) {
        buffer >> id;
        buffer >> dir;
        buffer >> x;
        buffer >> y;
        buffer >> hp;
    }
};

struct SC_CREATE_OTHER_CHARACTER {
    int id;
    char dir;
    short x;
    short y;
    char hp;

    void Serialize(CMessage& buffer) {
        buffer << id;
        buffer << dir;
        buffer << x;
        buffer << y;
        buffer << hp;
    }

    void Deserialize(CMessage& buffer) {
        buffer >> id;
        buffer >> dir;
        buffer >> x;
        buffer >> y;
        buffer >> hp;
    }
};

struct SC_DELETE_CHARACTER {
    int id;

    void Serialize(CMessage& buffer) {
        buffer << id;
    }

    void Deserialize(CMessage& buffer) {
        buffer >> id;
    }
};

struct CS_MOVE_START {
    char dir;
    short x;
    short y;

    void Serialize(CMessage& buffer) {
        buffer << dir;
        buffer << x;
        buffer << y;
    }

    void Deserialize(CMessage& buffer) {
        buffer >> dir;
        buffer >> x;
        buffer >> y;
    }
};

struct SC_MOVE_START {
    int id;
    char dir;
    short x;
    short y;

    void Serialize(CMessage& buffer) {
        buffer << id;
        buffer << dir;
        buffer << x;
        buffer << y;
    }

    void Deserialize(CMessage& buffer) {
        buffer >> id;
        buffer >> dir;
        buffer >> x;
        buffer >> y;
    }
};

struct CS_MOVE_STOP {
    char dir;
    short x;
    short y;

    void Serialize(CMessage& buffer) {
        buffer << dir;
        buffer << x;
        buffer << y;
    }

    void Deserialize(CMessage& buffer) {
        buffer >> dir;
        buffer >> x;
        buffer >> y;
    }
};

struct SC_MOVE_STOP {
    int id;
    char dir;
    short x;
    short y;

    void Serialize(CMessage& buffer) {
        buffer << id;
        buffer << dir;
        buffer << x;
        buffer << y;
    }

    void Deserialize(CMessage& buffer) {
        buffer >> id;
        buffer >> dir;
        buffer >> x;
        buffer >> y;
    }
};

struct CS_ATTACK1 {
    char dir;
    short x;
    short y;

    void Serialize(CMessage& buffer) {
        buffer << dir;
        buffer << x;
        buffer << y;
    }

    void Deserialize(CMessage& buffer) {
        buffer >> dir;
        buffer >> x;
        buffer >> y;
    }
};

struct SC_ATTACK1 {
    int id;
    char dir;
    short x;
    short y;

    void Serialize(CMessage& buffer) {
        buffer << id;
        buffer << dir;
        buffer << x;
        buffer << y;
    }

    void Deserialize(CMessage& buffer) {
        buffer >> id;
        buffer >> dir;
        buffer >> x;
        buffer >> y;
    }
};

struct CS_ATTACK2 {
    char dir;
    short x;
    short y;

    void Serialize(CMessage& buffer) {
        buffer << dir;
        buffer << x;
        buffer << y;
    }

    void Deserialize(CMessage& buffer) {
        buffer >> dir;
        buffer >> x;
        buffer >> y;
    }
};

struct SC_ATTACK2 {
    int id;
    char dir;
    short x;
    short y;

    void Serialize(CMessage& buffer) {
        buffer << id;
        buffer << dir;
        buffer << x;
        buffer << y;
    }

    void Deserialize(CMessage& buffer) {
        buffer >> id;
        buffer >> dir;
        buffer >> x;
        buffer >> y;
    }
};

struct CS_ATTACK3 {
    char dir;
    short x;
    short y;

    void Serialize(CMessage& buffer) {
        buffer << dir;
        buffer << x;
        buffer << y;
    }

    void Deserialize(CMessage& buffer) {
        buffer >> dir;
        buffer >> x;
        buffer >> y;
    }
};

struct SC_ATTACK3 {
    int id;
    char dir;
    short x;
    short y;

    void Serialize(CMessage& buffer) {
        buffer << id;
        buffer << dir;
        buffer << x;
        buffer << y;
    }

    void Deserialize(CMessage& buffer) {
        buffer >> id;
        buffer >> dir;
        buffer >> x;
        buffer >> y;
    }
};

struct SC_DAMAGE {
    int attack_id;
    int damage_id;
    char hp;

    void Serialize(CMessage& buffer) {
        buffer << attack_id;
        buffer << damage_id;
        buffer << hp;
    }

    void Deserialize(CMessage& buffer) {
        buffer >> attack_id;
        buffer >> damage_id;
        buffer >> hp;
    }
};

struct SC_SYNC {
    int id;
    short x;
    short y;

    void Serialize(CMessage& buffer) {
        buffer << id;
        buffer << x;
        buffer << y;
    }

    void Deserialize(CMessage& buffer) {
        buffer >> id;
        buffer >> x;
        buffer >> y;
    }
};

