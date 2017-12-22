//
// Created by cyborg on 11/11/17.
//

#ifndef XML_DOM_HPP
#define XML_DOM_HPP

#include <memory>
#include <list>
#include <map>
#include <stack>
#include <functional>
#include <sstream>
#include "Errors.hpp"

namespace XML::DOM
{

class Node
{
public:
    virtual ~Node() = 0;

    enum class Type
    {
        INVALID_NODE,
        ELEMENT_NODE,
        TEXT_NODE,
        CDATA_SECTION_NODE,
        COMMENT_NODE,
        DOCUMENT_NODE
    };

    explicit Node(Type type = Type::INVALID_NODE, const std::string &name = "", const std::string &value = "");
    virtual void append_child(Node *new_child);
    void insert_before(Node *new_child, Node *ref_child);
    void remove_child(Node *old_child);
    bool has_child_nodes();
    bool is_ancestor(Node *other);
    std::list<class Element*> get_elements_by_tag_name(const std::string &tag_name);

    Type type;
    std::string name;
    std::string value;
    std::list<std::unique_ptr<Node>> child_nodes;
    Node* parent_node;
    Node* previous_sibling;
    Node* next_sibling;

    virtual std::string serialize(size_t tab_size, size_t level);

    // pre-order iterator
    class iterator
    {
        Node* node;
        std::stack<Node*> stack;
    public:
        using difference_type = size_t;
        using value_type = Node*;
        using pointer = Node**;
        using reference = Node*&;
        using iterator_category = std::forward_iterator_tag;

        explicit iterator(Node* node) : node(node) { stack.push(node); }
        iterator& operator++()
        {
            if (stack.empty()) {
                node = nullptr;
            } else {
                node = stack.top();
                stack.pop();
                auto &children = node->child_nodes;
                for (auto it = children.rbegin(); it != children.rend(); it++)
                    stack.push(it->get());
            }
            return *this;
        }

        iterator operator++(int) { iterator retval = *this; ++(*this); return retval; }
        bool operator==(iterator other) const { return node == other.node; }
        bool operator!=(iterator other) const { return !(*this == other); }
        reference operator*() { return node; }
    };

    iterator begin() { return iterator(this); }
    iterator end()   { return iterator(nullptr); }
};

class Element : public Node
{
public:
    explicit Element(const std::string &tag_name);

    bool has_attribute(const std::string &name);
    void set_attribute(const std::string &name, const std::string &value);
    std::string attribute(const std::string &name);
    void remove_attribute(const std::string &name);

    std::string serialize(size_t tab_size, size_t level) override;
protected:
    std::map<std::string, std::string> attributes;
};

class Text : public Node
{
public:
    explicit Text(const std::string &text = "");

    void append_child(Node *new_child) override;

    std::string serialize(size_t tab_size, size_t level) override;
};

class CDATASection : public Node
{
public:
    explicit CDATASection(const std::string &text = "");

    void append_child(Node *new_child) override;

    std::string serialize(size_t tab_size, size_t level) override;
};

class Comment : public Node
{
public:
    explicit Comment(const std::string &text = "");

    void append_child(Node *new_child) override;

    std::string serialize(size_t tab_size, size_t level) override;
};

class Document : public Node
{
public:
    explicit Document();

    std::string xml_prolog;
    std::string doctype;
    Element* root_element;

    void append_child(Node *new_child) override;

    std::string serialize(size_t tab_size = 0);
};

} // namespace XML::DOM

#endif //XML_DOM_HPP
