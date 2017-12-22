//
// Created by cyborg on 12/4/17.
//

#include "DOM.hpp"

namespace XML::DOM
{

Node::~Node() = default;

Node::Node(Type type, const std::string &name, const std::string &value)
        : type(type),
          name(name),
          value(value),
          parent_node(nullptr),
          previous_sibling(nullptr),
          next_sibling(nullptr) {}

void Node::append_child(Node *new_child)
{
    if (new_child and not is_ancestor(new_child)) {
        if (new_child->type == Type::INVALID_NODE or new_child->type == Type::DOCUMENT_NODE)
            throw DOMError("Cannot append nodes of this type");
        new_child->parent_node = this;
        if (has_child_nodes()) {
            new_child->previous_sibling = child_nodes.back().get();
            new_child->next_sibling = nullptr;
        }
        child_nodes.emplace_back(new_child);
    } else {
        throw DOMError("Cannot append a null node");
    }
}

bool Node::has_child_nodes()
{
    return !child_nodes.empty();
}

bool Node::is_ancestor(Node *other)
{
    if (other == nullptr)
        return false;

    auto curr = other->parent_node;
    while (curr)
        if (curr == this)
            return true;

    return false;
}

std::list<Element *> Node::get_elements_by_tag_name(const std::string &tag_name)
{
    std::list<Element *> elements;
    for (auto&& node : *this)
        if (node->type == Type::ELEMENT_NODE and (node->name == tag_name or tag_name == "*"))
            elements.push_back(static_cast<Element *>(node));
    return elements;
}

void Node::insert_before(Node *new_child, Node *ref_child)
{
    if (new_child and ref_child) {
        if (ref_child->parent_node != this)
            throw DOMError("ref_child is not a child of this node");

        for (auto it = child_nodes.begin(); it != child_nodes.end(); it++) {
            auto &node = *it;
            if (node.get() == ref_child) {
                auto before_new = node->previous_sibling;
                new_child->parent_node = this;
                new_child->previous_sibling = before_new;
                new_child->next_sibling = ref_child;
                if (before_new)
                    before_new->next_sibling = new_child;
                node->previous_sibling = new_child;
                child_nodes.emplace(it, new_child);
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
        if (old_child->parent_node != this)
            throw DOMError("old_child is not a child of this node");

        for (auto it = child_nodes.begin(); it != child_nodes.end(); it++) {
            auto &node = *it;
            if (node.get() == old_child) {
                if (old_child->previous_sibling)
                    old_child->previous_sibling->next_sibling = old_child->next_sibling;
                if (old_child->next_sibling)
                    old_child->next_sibling->previous_sibling = old_child->previous_sibling;
                child_nodes.erase(it);
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

Document::Document() : Node(Type::DOCUMENT_NODE), root_element(nullptr) {}

void Document::append_child(Node *new_child)
{
    if (new_child == nullptr)
        throw DOMError("Cannot append a null node");

    if (new_child->type != Type::ELEMENT_NODE and new_child->type != Type::COMMENT_NODE)
        throw DOMError("Document node cannot have child of this type");

    if (new_child->type == Type::ELEMENT_NODE) {
        int root_count = 0;
        for (auto&& node : child_nodes)
            if (node->type == Type::ELEMENT_NODE)
                root_count++;
        if (root_count != 0)
            throw DOMError("Document node can't have more than one root element");
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

std::string Element::serialize(size_t tab_size, size_t level)
{
    std::string out;
    std::string tab(tab_size * level, ' ');
    out += tab + '<' + name;
    for (auto &kv : attributes)
        out += ' ' + kv.first + "=\"" + kv.second + '"';
    if (has_child_nodes()) {
        out += ">\n";
        for (auto &node : child_nodes)
            out += node->serialize(tab_size, level + 1);
        out += tab + "</" + name + ">\n";
    } else {
        out += "/>\n";
    }
    return out;
}

std::string Text::serialize(size_t tab_size, size_t level)
{
    std::string out;
    std::string tab(tab_size * level, ' ');
    std::istringstream ss(value);
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

std::string Comment::serialize(size_t tab_size, size_t level)
{
    std::string out;
    std::string tab(tab_size * level, ' ');
    out += tab + "<!--";
    std::istringstream ss(value);
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

std::string CDATASection::serialize(size_t tab_size, size_t level)
{
    std::string out;
    std::string tab(tab_size * level, ' ');
    out += tab + "<![CDATA[";
    std::istringstream ss(value);
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


std::string Document::serialize(size_t tab_size)
{
    std::string out;
    if (!xml_prolog.empty())
        out += xml_prolog + "\n";
    if (!doctype.empty())
        out += doctype + "\n";
    for (auto&& child : child_nodes)
        out += child->serialize(tab_size, 0);
    return out;
}

} // namespace XML::DOM
