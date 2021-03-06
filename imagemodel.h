#ifndef IMAGEMODEL_H
#define IMAGEMODEL_H

#include "codeblock.h"
#include "imageelement.h"

#include "zbinary.h"
#include "zmap.h"
#include "zlist.h"
#include "zstack.h"

#include <capstone/capstone.h>

#define HEX(A) (ZString::ItoS((zu64)(A), 16))
#define HEX_PAD(A, B) (ZString::ItoS((zu64)(A), 16, (B)))
//#define HEX(A) (ZString("0x")+ZString::ItoS((A), 16))

using namespace LibChaos;

class ImageModel {
public:
    enum labeltype {
        CODE,
        DATA,
    };

    enum nametype {
        NAMED,
        CALL,
        SWITCH,
        JUMP,
        LDR,
        AUTO
    };

    struct Label {
        labeltype ltype;
        nametype ntype;
        ZString str;
        bool thumbfunc;
    };

    enum datatype {
        VALUE,
        CPTR,
        DPTR,
    };

    struct DataWord {
        datatype type;
        zu64 data;
    };

public:
    ImageModel(bool equiv, bool verbose);
    ~ImageModel();

    //! Load a binary image at the given offset.
    void loadImage(const ZBinary &bin, zu64 offset);
    //! Add a code entry point in the provided binary.
    zu64 addEntry(zu64 addr, ZString name = ZString(), ArZ params = ArZ());
    zu64 addCodePointer(zu64 addr, ZString name = ZString());

    zu64 addData(zu64 addr, ZString name = ZString(), zu64 words = 0);
    zu64 addDataPointer(zu64 addr, ZString name = ZString(), zu64 words = 0);

    zu64 disassembleAddress(zu64 addr, ZStack<ZString> stack = ZStack<ZString>());

    ZBinary makeCode(bool offsets = false, bool annotate = false);

    void addLabel(zu64 addr, labeltype ltype, nametype ntype, ZString name = ZString(), bool thumbfunc = false);

    void setSwitchLen(zu64 addr, zu64 len);
    void setForced(zu64 addr, labeltype type);
    void addAnnotation(zu64 addr, ZString note);

    zu64 numInsns() const;

private:
    zu64 _addrToOffset(zu64 addr) const;
    zu64 _offsetToAddr(zu64 offset) const;

public:
    bool equiv;
    bool verbose;

    //! Image base offset.
    zu64 base;
    //! Image data.
    ZBinary image;

    //! Code and data labels.
    ZMap<zu64, Label> labels;
    ZMap<zu64, ZString> lparams;
    //! Disassembled instructions.
    ZMap<zu64, CodeBlock::Insn> insns;
    //! Data referenced by code.
    ZMap<zu64, DataWord> data;
    ZMap<zu64, labeltype> forcetype;

    ZMap<zu64, ZPointer<CodeBlock>> code;
    //! Additional annotations.
    ZMap<zu64, ZString> annotations;

    ZMap<zu64, zu64> switches;

    // Capstone
    csh handle;
    cs_err err;
};

#endif // IMAGEMODEL_H
