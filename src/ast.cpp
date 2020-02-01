
#include "polar-lang/ast.h"
#include <iostream>

namespace polar {

json Node::make_node(json js) const {
    js["node"] = type_;
    return js;
}

IntTerm* IntTerm::do_clone() const {
    return new IntTerm{value_};
}

json IntTerm::dump() const {
    return value_;
}

StringTerm* StringTerm::do_clone() const {
    return new StringTerm{value_};
}

json StringTerm::dump() const {
    return value_;
}

KleineTerm* KleineTerm::do_clone() const {
    return new KleineTerm{};
}

json KleineTerm::dump() const {
    return "*";
}

ModuleTerm* ModuleTerm::do_clone() const {
    return new ModuleTerm{};
}

json TermNode::dump() const {
    return make_node({
        {"value", t_->dump()},
        {"type", t_->type()},
    });
}

CallFuncNode::CallFuncNode(String name, Seq<UNode> args)
:       Node(CallFuncNode::TYPE),
        name_(std::move(name)),
        args_(std::move(args)) {
}

CallFuncNode::CallFuncNode(UTerm term, Seq<UNode> args)
:       Node(CallFuncNode::TYPE),
        term_(std::move(term)),
        args_(std::move(args)) {
}

RegexpRuleNode::RegexpRuleNode(UNodeSeq&& args)
:       Node(RegexpRuleNode::TYPE),
        args_(std::move(args)) {
}

json RegexpRuleNode::dump() const {
    return make_node({
        {"args", to_json(args_)},
    });
}

ResponseNode::ResponseNode(UNodeSeq&& args)
:       Node(ResponseNode::TYPE),
        args_(std::move(args)) {
}

json ResponseNode::dump() const {
    return make_node({
         {"args", to_json(args_)},
    });
}

json to_json(const UNodeSeq& seq) {
    json args = json::array();
    for (const auto& a : seq) {
        args.push_back(a->dump());
    }
    return args;
}

} // namespace polar {