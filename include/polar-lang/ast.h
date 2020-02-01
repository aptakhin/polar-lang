#pragma once

#include <unordered_map>
#include <iostream>

#include <nlohmann/json.hpp>

#include "polar-lang/common.h"

namespace polar {

using json = nlohmann::json;

enum class ENode {
    UNDEFINED,
    NAME,
    TERM,
    CALL_FUNC,
    REGEXP_RULE,
};

NLOHMANN_JSON_SERIALIZE_ENUM(ENode, {
    {ENode::UNDEFINED, "undefined"},
    {ENode::NAME, "name"},
    {ENode::TERM, "term"},
    {ENode::CALL_FUNC, "call_func"},
    {ENode::REGEXP_RULE, "regexp_rule"},
})

class Node {
public:
    Node(const ENode type)
    :   type_(type) {
    }

    virtual ~Node() = default;

    ENode type() const { return type_; }

    virtual bool is_abstract() const { return true; };

    virtual json dump() const { return json{}; }
//
//    virtual Node* do_clone() const = 0;

    template <class T>
    T* as() {
        if (type() != T::TYPE) {
            std::cerr << "ERROR casting" << std::endl;
        }
        return static_cast<T*>(this);
    }

    template <class T>
    const T* as() const {
        if (type() != T::TYPE) {
            std::cerr << "ERROR casting" << std::endl;
        }
        return static_cast<const T*>(this);
    }

    template <class T>
    static T* as(U<Node>& holder) {
        if (holder->type() != T::TYPE) {
            std::cerr << "ERROR casting" << std::endl;
        }
        return static_cast<T*>(holder.get());
    }

    template <class T>
    static const T* as(const U<Node>& holder) {
        if (holder->type() != T::TYPE) {
            // ERROR!!
            std::cerr << "ERROR casting" << std::endl;
        }
        return static_cast<const T*>(holder.get());
    }
protected:

    json make_node(json value) const;

protected:
    ENode type_;
};
using UNode = U<Node>;

enum class ETerm {
    UNDEFINED,
    INT,
    STRING,
    KLEINE,
    MODULE,
    FUNC,
};

NLOHMANN_JSON_SERIALIZE_ENUM(ETerm, {
    {ETerm::UNDEFINED, "undefined"},
    {ETerm::INT, "int"},
    {ETerm::STRING, "string"},
    {ETerm::KLEINE, "kleine"},
    {ETerm::MODULE, "module"},
    {ETerm::FUNC, "func"},
})

class Term {
public:
    Term(ETerm type)
    :   type_(type) {
    }

    virtual ~Term() {};

    ETerm type() const { return type_; }

    virtual Term* do_clone() const = 0;

    virtual json dump() const { return json{}; }

    template <class T>
    T* as() {
        if (type() != T::TYPE) {
            // ERROR!!
            std::cerr << "ERROR casting" << std::endl;
        }
        return static_cast<T*>(this);
    }

    template <class T>
    const T* as() const {
        if (type() != T::TYPE) {
            // ERROR!!
            std::cerr << "ERROR casting" << std::endl;
        }
        return static_cast<const T*>(this);
    }

private:
    const ETerm type_ = ETerm::UNDEFINED;
};
using UTerm = U<Term>;

class TermContract {
public:
    virtual ~TermContract() {}

    virtual bool match(ETerm type) const = 0;
    virtual bool match(Term* term) const = 0;
};

class SimpleTypeTermContract : public TermContract {
public:
    SimpleTypeTermContract(ETerm type)
    :   type_(type) {
    }

    bool match(ETerm type) const override {
        return type == type_;
    }

    bool match(Term* term) const override {
        return term->type() == type_;
    }

private:
    ETerm type_;
};

class IntTerm : public Term {
public:
    using Self = IntTerm;

    const static ETerm TYPE = ETerm::INT;

    IntTerm(int value)
    :       Term(Self::TYPE),
            value_(value) {
    }

    int value() const { return value_; }

    virtual IntTerm* do_clone() const override;

    json dump() const override;

private:
    const int value_;
};

class StringTerm : public Term {
public:
    using Self = StringTerm;

    const static ETerm TYPE = ETerm::STRING;

    StringTerm(String value)
            :       Term(Self::TYPE),
                    value_(std::move(value)) {
    }

    const String& value() const { return value_; }

    virtual StringTerm* do_clone() const override;

    json dump() const override;

private:
    const String value_;
};

class KleineTerm : public Term {
public:
    using Self = KleineTerm;

    const static ETerm TYPE = ETerm::KLEINE;

    KleineTerm()
    :       Term(Self::TYPE) {
    }

    virtual KleineTerm* do_clone() const override;

    json dump() const override;

private:
};

class ModuleTerm : public Term {
public:
    using Self = ModuleTerm;

    const static ETerm TYPE = ETerm::MODULE;

    ModuleTerm()
    :       Term(Self::TYPE) {

    }

    void add(const String& name, UTerm&& term) {
        context_.insert(std::make_pair(name, std::move(term)));
    }

    virtual ModuleTerm* do_clone() const override;

private:
    std::unordered_multimap<String, UTerm> context_;
};

class State {
public:
    friend class Runner;

private:
    ModuleTerm root_;
};

class FuncArgumentsItem {
public:

private:
    String name_;
    U<TermContract> type_req_;
};

class FuncArguments {
private:
    std::vector<FuncArgumentsItem> args_;
};
//
//class FuncNode : public Node {
//private:
//    FuncArguments args_;
//
//    //std::unordered_map<String, U<Term>> context_;
//
//    std::vector<U<Node>> flow_;
//};

class NameNode : public Node {
public:
    using Self = NameNode;
    static const ENode TYPE = ENode::NAME;

    NameNode(const String& name)
    :   Node(Self::TYPE),
        name_(name) {
    }

//    virtual NameNode* do_clone() const override;

private:
    const String name_;
};

class TermNode : public Node {
public:
    using Self = TermNode;
    static const ENode TYPE = ENode::TERM;

    TermNode(UTerm t)
    :   Node(Self::TYPE),
        t_(std::move(t)) {
    }

    json dump() const override;

    const Term* term() const { return t_.get(); }

private:
    UTerm t_;
};


using UNodeSeq = Seq<UNode>;

class CallFuncNode : public Node {
public:
    static const ENode TYPE = ENode::CALL_FUNC;

    CallFuncNode(String name, UNodeSeq args);
    CallFuncNode(UTerm term, UNodeSeq args);

    const String& name() const { return name_; }
    const Term* term() const { return term_.get(); }
    const UNodeSeq& args() const { return args_; }

private:
    String name_;
    UTerm term_;
    UNodeSeq args_;
};

class RegexpRuleNode : public Node {
public:
    static const ENode TYPE = ENode::REGEXP_RULE;

    RegexpRuleNode(UNodeSeq&& args);

    const UNodeSeq& args() const { return args_; }

    json dump() const override;

private:
    UNodeSeq args_;
};

json to_json(const UNodeSeq& seq);

} // namespace polar {
