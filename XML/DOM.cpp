//
// Created by cyborg on 12/4/17.
//

#include "DOM.hpp"

namespace XML::DOM
{

Node::~Node() = default;

std::string Node::type_name()
{
    switch (type_) {
    case Type::INVALID_NODE:
        return "Invalid";
    case Type::ELEMENT_NODE:
        return "Element";
    case Type::TEXT_NODE:
        return "Text";
    case Type::CDATA_SECTION_NODE:
        return "CDATA Section";
    case Type::COMMENT_NODE:
        return "Comment";
    case Type::DOCUMENT_NODE:
        return "Document";
    }
}

Node::Node(Type type, const std::string &name, const std::string &value)
        : type_(type),
          name_(name),
          value_(value),
          parent_node_(nullptr),
          previous_sibling_(nullptr),
          next_sibling_(nullptr) {}

void Node::append_child(Node *new_child)
{
    if (new_child and not is_ancestor(new_child)) {
        if (new_child->type_ == Type::INVALID_NODE or new_child->type_ == Type::DOCUMENT_NODE)
            throw DOMError("Cannot append nodes of this type");
        new_child->parent_node_ = this;
        if (has_child_nodes()) {
            new_child->previous_sibling_ = child_nodes_.back().get();
            new_child->next_sibling_ = nullptr;
        }
        child_nodes_.emplace_back(new_child);
    } else {
        throw DOMError("Cannot append a null node");
    }
}

bool Node::has_child_nodes()
{
    return !child_nodes_.empty();
}

bool Node::is_ancestor(Node *other)
{
    if (other == nullptr)
        return false;

    auto curr = other->parent_node_;
    while (curr)
        if (curr == this)
            return true;

    return false;
}

std::list<Element *> Node::get_elements_by_tag_name(const std::string &tag_name)
{
    std::list<Element *> elements;
    for (auto&& node : *this)
        if (node->type_ == Type::ELEMENT_NODE and (node->name_ == tag_name or tag_name == "*"))
            elements.push_back(dynamic_cast<Element *>(node));
    return elements;
}

const std::list<std::unique_ptr<Node> > &Node::siblings() const
{
    return parent_node_->child_nodes_;
}

void Node::insert_before(Node *new_child, Node *ref_child)
{
    if (new_child and ref_child and not is_ancestor(new_child)) {
        if (ref_child->parent_node_ != this)
            throw DOMError("ref_child is not a child of this node");

        if (new_child->type_ == Type::INVALID_NODE or new_child->type_ == Type::DOCUMENT_NODE)
            throw DOMError("Cannot append nodes of this type");

        for (auto it = child_nodes_.begin(); it != child_nodes_.end(); it++) {
            auto &node = *it;
            if (node.get() == ref_child) {
                auto before_new = node->previous_sibling_;
                new_child->parent_node_ = this;
                new_child->previous_sibling_ = before_new;
                new_child->next_sibling_ = ref_child;
                if (before_new)
                    before_new->next_sibling_ = new_child;
                node->previous_sibling_ = new_child;
                child_nodes_.emplace(it, new_child);
                return;
            }
        }
    } else if (!new_child) {
        throw DOMError("Cannot insert a null node");
    } else {
        throw DOMError("Cannot insert before a null node");
    }
}

void Node::remove_child(Node *old_child)
{
    if (old_child) {
        if (old_child->parent_node_ != this)
            throw DOMError("old_child is not a child of this node");

        for (auto it = child_nodes_.begin(); it != child_nodes_.end(); it++) {
            auto &node = *it;
            if (node.get() == old_child) {
                if (old_child->previous_sibling_)
                    old_child->previous_sibling_->next_sibling_ = old_child->next_sibling_;
                if (old_child->next_sibling_)
                    old_child->next_sibling_->previous_sibling_ = old_child->previous_sibling_;
                child_nodes_.erase(it);
                return;
            }
        }
    } else {
        throw DOMError("Cannot remove a null node");
    }
}

Element::Element(const std::string &tag_name) : Node(Type::ELEMENT_NODE, tag_name, "") {}
Text::Text(const std::string &text) : Node(Type::TEXT_NODE, "", text) {}

void Text::append_child(Node *new_child)
{
    throw DOMError("Text node cannot have child nodes");
}

CDATASection::CDATASection(const std::string &text) : Node(Type::CDATA_SECTION_NODE, "", text) {}

void CDATASection::append_child(Node *new_child)
{
    throw DOMError("CDATASection node cannot have child nodes");
}

Comment::Comment(const std::string &text) : Node(Type::COMMENT_NODE, "", text) {}

void Comment::append_child(Node *new_child)
{
    throw DOMError("Comment node cannot have child nodes");
}

Document::Document() : Node(Type::DOCUMENT_NODE), root_element_(nullptr) {}

void Document::append_child(Node *new_child)
{
    if (new_child == nullptr)
        throw DOMError("Cannot append a null node");

    if (new_child->type() != Type::ELEMENT_NODE and new_child->type() != Type::COMMENT_NODE)
        throw DOMError("Document node cannot have child of this type");

    if (new_child->type() == Type::ELEMENT_NODE) {
        int root_count = 0;
        for (auto&& node : child_nodes_)
            if (node->type() == Type::ELEMENT_NODE)
                root_count++;
        if (root_count != 0)
            throw DOMError("Document node can't have more than one root element");
        root_element_ = dynamic_cast<Element*>(new_child);
    }

    Node::append_child(new_child);
}

void Element::set_attribute(const std::string &name, const std::string &value)
{
    attributes[name] = value;
}

bool Element::has_attribute(const std::string &name)
{
    return attributes.find(name) != attributes.end();
}

std::string Element::attribute(const std::string &name)
{
    if (has_attribute(name))
        return attributes[name];
    else
        return std::string();
}

void Element::remove_attribute(const std::string &name)
{
    attributes.erase(name);
}

std::string Node::serialize(size_t tab_size, size_t level)
{
    return std::__cxx11::string();
}

Node *Node::child_at(size_t index)
{
    if (child_nodes_.size() <= index)
        return nullptr;

    auto it = child_nodes_.begin();
    std::advance(it, index);
    return it->get();
}

size_t Node::child_num()
{
    size_t i = 0;
    for (auto&& node : siblings()) {
        if (node.get() == this)
            return i;
        i++;
    }
    return i;
}

std::string Node::text_content()
{
    return value_;
}

Node::Node(Node&& other) noexcept : type_(other.type_), name_(std::move(other.name_)), value_(std::move(other.value_)),
                                    child_nodes_(std::move(other.child_nodes_)),
                                    parent_node_(other.parent_node_), previous_sibling_(other.previous_sibling_),
                                    next_sibling_(other.next_sibling_)
{
    for (auto&& node : child_nodes_)
        node->parent_node_ = this;

    other.type_ = Type::INVALID_NODE;
    other.parent_node_ = nullptr;
    other.previous_sibling_ = nullptr;
    other.next_sibling_ = nullptr;
}

void Node::set_text_content(const std::string &text)
{
    value_ = text;
}

const std::list<std::unique_ptr<Node>> &Node::child_nodes() const
{
    return child_nodes_;
}

Node& Node::operator=(Node &&other) noexcept
{
    type_ = other.type_;
    name_ = std::move(other.name_);
    value_ = std::move(other.value_);
    child_nodes_ = std::move(other.child_nodes_);
    parent_node_ = other.parent_node_;
    previous_sibling_ = other.previous_sibling_;
    next_sibling_ = other.next_sibling_;

    other.type_ = Type::INVALID_NODE;
    other.parent_node_ = nullptr;
    other.previous_sibling_ = nullptr;
    other.next_sibling_ = nullptr;

    return *this;
}

Node::Type Node::type() const
{
    return type_;
}

const std::string &Node::name() const
{
    return name_;
}

const std::string &Node::value() const
{
    return value_;
}

Node *Node::parent_node() const
{
    return parent_node_;
}

Node *Node::previous_sibling() const
{
    return previous_sibling_;
}

Node *Node::next_sibling() const
{
    return next_sibling_;
}

void Node::set_name(const std::string &name)
{
    Lexer::validate_name(name);
    Node::name_ = name;
}

void Node::set_value(const std::string &value)
{
    for (auto &&ch : value)
        if (ch == '>' or ch == '<')
            throw SyntaxError("Unexpected symbol in node value");
    Node::value_ = value;
}

std::string Element::serialize(size_t tab_size, size_t level)
{
    std::string out;
    std::string tab(tab_size * level, ' ');
    out += tab + '<' + name_;
    for (auto &kv : attributes)
        out += ' ' + kv.first + "=\"" + kv.second + '"';
    if (has_child_nodes()) {
        out += '>';
        if (child_nodes_.size() == 1 and child_nodes_.front()->type() == Type::TEXT_NODE) {
            out += text_content();
            out += "</" + name_ + ">\n";
        } else {
            out += '\n';
            for (auto &node : child_nodes_)
                out += node->serialize(tab_size, level + 1);
            out += tab + "</" + name_ + ">\n";
        }
    } else {
        out += "/>\n";
    }
    return out;
}

std::string Element::text_content()
{
    std::string buffer;
    if (child_nodes_.size() == 1 and child_nodes_.front()->type() == Type::TEXT_NODE) {
        buffer = child_nodes_.front()->value();
    } else {
        for (auto&& node : *this)
            if (node->type() == Type::TEXT_NODE)
                buffer += node->value() + " ";
    }
    return buffer;
}

void Element::set_text_content(const std::string &text)
{
    auto text_node = new Text;
    text_node->set_text_content(text);
    child_nodes_.clear();
    append_child(text_node);
}

Element::Element(Element &&other) noexcept : attributes(std::move(other.attributes)), Node(std::move(other)) {}

Element &Element::operator=(Element &&other) noexcept
{
    Node::operator=(std::move(other));
    attributes = std::move(other.attributes);
    return *this;
}

std::string Text::serialize(size_t tab_size, size_t level)
{
    std::string out;
    std::string tab(tab_size * level, ' ');
    std::istringstream ss(value_);
    while (ss) {
        std::string temp;
        std::getline(ss, temp, '\n');
        size_t first = temp.find_first_not_of(" \t\n\r");
        if (first == std::string::npos)
            continue;
        temp = temp.substr(first, temp.size());
        if (!temp.empty())
            out += tab + temp + "\n";
    }
    return out;
}

void Text::set_text_content(const std::string &text)
{
    for (auto &&ch : text)
        if (ch == '>' or ch == '<')
            throw SyntaxError("Unexpected symbol in text content");
    Node::set_text_content(text);
}

void Text::insert_before(Node *new_child, Node *ref_child)
{
    throw DOMError("Text node cannot have child nodes");
}

std::string Comment::serialize(size_t tab_size, size_t level)
{
    std::string out;
    std::string tab(tab_size * level, ' ');
    out += tab + "<!--";
    std::istringstream ss(value_);
    std::string temp;
    std::getline(ss, temp, '\n');
    size_t first = temp.find_first_not_of(" \t\n\r");
    if (first != std::string::npos)
        out += temp.substr(first, temp.size());
    while (ss) {
        std::string temp;
        std::getline(ss, temp, '\n');
        size_t first = temp.find_first_not_of(" \t\n\r");
        if (first == std::string::npos)
            continue;
        temp = temp.substr(first, temp.size());
        if (!temp.empty()) {
            out += '\n';
            out += tab + temp;
        }
    }
    out += "-->\n";
    return out;
}

void Comment::insert_before(Node *new_child, Node *ref_child)
{
    throw DOMError("Comment node cannot have child nodes");
}

std::string CDATASection::serialize(size_t tab_size, size_t level)
{
    std::string out;
    std::string tab(tab_size * level, ' ');
    out += tab + "<![CDATA[";
    std::istringstream ss(value_);
    std::string temp;
    std::getline(ss, temp, '\n');
    size_t first = temp.find_first_not_of(" \t\n\r");
    if (first != std::string::npos)
        out += temp.substr(first, temp.size());
    while (ss) {
        std::string temp;
        std::getline(ss, temp, '\n');
        size_t first = temp.find_first_not_of(" \t\n\r");
        if (first == std::string::npos)
            continue;
        temp = temp.substr(first, temp.size());
        if (!temp.empty()) {
            out += '\n';
            out += tab + temp;
        }
    }
    out += "]]>\n";
    return out;
}

void CDATASection::insert_before(Node *new_child, Node *ref_child)
{
    throw DOMError("CDATA Section node cannot have child nodes");
}


std::string Document::serialize(size_t tab_size)
{
    std::string out;
    if (!xml_prolog_.empty())
        out += xml_prolog_ + "\n";
    if (!doctype_.empty())
        out += doctype_ + "\n";
    for (auto&& child : child_nodes_)
        out += child->serialize(tab_size, 0);
    return out;
}

Document::Document(Document &&other) noexcept : xml_prolog_(std::move(other.xml_prolog_)),
                                                doctype_(std::move(other.doctype_)),
                                                Node(std::move(other)) {}

Document &Document::operator=(Document &&other) noexcept
{
    Node::operator=(std::move(other));
    xml_prolog_ = std::move(other.xml_prolog_);
    doctype_ = std::move(other.doctype_);
    root_element_ = other.root_element_;
    other.root_element_ = nullptr;
    return *this;
}

const std::string &Document::xml_prolog() const
{
    return xml_prolog_;
}

void Document::set_xml_prolog(const std::string &xml_prolog)
{
    Document::xml_prolog_ = xml_prolog;
}

const std::string &Document::doctype() const
{
    return doctype_;
}

void Document::set_doctype(const std::string &doctype)
{
    Document::doctype_ = doctype;
}

Element *Document::root_element() const
{
    return root_element_;
}

void Document::insert_before(Node *new_child, Node *ref_child)
{
    if (new_child == nullptr)
        throw DOMError("Cannot append a null node");

    if (new_child->type() != Type::ELEMENT_NODE and new_child->type() != Type::COMMENT_NODE)
        throw DOMError("Document node cannot have child of this type");

    if (new_child->type() == Type::ELEMENT_NODE) {
        int root_count = 0;
        for (auto&& node : child_nodes_)
            if (node->type() == Type::ELEMENT_NODE)
                root_count++;
        if (root_count != 0)
            throw DOMError("Document node can't have more than one root element");
        root_element_ = dynamic_cast<Element*>(new_child);
    }

    Node::insert_before(new_child, ref_child);
}

} // namespace XML::DOM
