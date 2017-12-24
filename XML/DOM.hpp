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
#include "Lexer.hpp"

namespace XML::DOM
{

class Node
{
public:
    virtual ~Node() = 0;
    /// Move constructor
    /// \param other Node to move
    Node(Node&& other) noexcept;
    /// Move operator=
    /// \param other Node to move
    /// \return Ref to *this
    Node& operator=(Node&& other) noexcept;

    enum class Type
    {
        INVALID_NODE,
        ELEMENT_NODE,
        TEXT_NODE,
        CDATA_SECTION_NODE,
        COMMENT_NODE,
        DOCUMENT_NODE
    };

    /// Node constructor
    /// \param type Node type
    /// \param name Node name
    /// \param value Node value
    explicit Node(Type type = Type::INVALID_NODE, const std::string &name = "", const std::string &value = "");

    /// Append a child to this node
    /// \param new_child Child to append
    virtual void append_child(Node *new_child);

    /// Insert a child before ref_child
    /// \param new_child Child to insert
    /// \param ref_child Insert before this child
    virtual void insert_before(Node *new_child, Node *ref_child);

    /// Remove a child of this node
    /// \param old_child Pointer to child
    void remove_child(Node *old_child);

    /// Check whether this node has children
    /// \return True if has child nodes
    bool has_child_nodes();

    /// Check whether this node is an ancestor of other node
    /// \param other Pointer of node to check
    /// \return True if this is an ancestor of other node
    bool is_ancestor(Node *other);

    /// Search ancestor elements by tag name
    /// \param tag_name Tag name (Wildcard "*" to get every single element)
    /// \return List of elements
    std::list<class Element*> get_elements_by_tag_name(const std::string &tag_name);

    /// Serialize this node and descendants
    /// \param tab_size Size of one tab in spaces
    /// \param level Current level of indent
    /// \return String representation of this node and descendants
    virtual std::string serialize(size_t tab_size, size_t level);

    /// Returns pointer to child by index
    /// \param index Index
    /// \return Pointer to child
    Node* child_at(size_t index);

    /// Returns this nodes position in the list of siblings (i.e. 0 if it's the parents first child)
    /// \return Position
    size_t child_num();

    // getters

    /// Returns text content of this child
    /// \return Text content
    virtual std::string text_content();

    /// Returns string representation of this nodes type
    /// \return Node type in string format
    std::string type_name();

    /// Returns this nodes type
    /// \return Node type
    Type type() const;

    /// Returns this nodes name
    /// \return Node name
    const std::string &name() const;

    /// Returns this nodes value
    /// \return Node value
    const std::string &value() const;

    /// Returns a list of this nodes children
    /// \return List of children
    const std::list<std::unique_ptr<Node>> &child_nodes() const;

    /// Returns a list of this nodes siblings (including itself)
    /// \return List of sibling
    const std::list<std::unique_ptr<Node>> &siblings() const;

    /// Returns a pointer to nodes parent
    /// \return Pointer to parent
    Node *parent_node() const;

    /// Returns a pointer to nodes previous sibling
    /// \return Pointer to previous sibling
    Node *previous_sibling() const;

    /// Returns a pointer to nodes next sibling
    /// \return Pointer to next sibling
    Node *next_sibling() const;

    // setters

    /// Sets text content of this node
    /// \param text New text content
    virtual void set_text_content(const std::string& text);

    /// Sets nodes name
    /// \param name New node name
    void set_name(const std::string &name);

    /// Sets nodes value
    /// \param value New node value
    void set_value(const std::string &value);

    // pre-order (tree order) iterator
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
                auto &children = node->child_nodes_;
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

    /// Pre-order (tree order) iterator to this node
    /// \return Begin iterator
    iterator begin() { return iterator(this); }

    /// Pre-order (tree order) iterator to nullptr
    /// \return End iterator
    iterator end()   { return iterator(nullptr); }

protected:
    Type type_;
    std::string name_;
    std::string value_;
    std::list<std::unique_ptr<Node>> child_nodes_;
    Node* parent_node_;
    Node* previous_sibling_;
    Node* next_sibling_;
};

class Element : public Node
{
public:
    /// Element constructor
    /// \param tag_name Tag name
    explicit Element(const std::string &tag_name);

    /// Element move constructor
    /// \param other Element to move
    Element(Element&& other) noexcept;

    /// Check whether this element has attribute
    /// \param name Name of the attribute
    /// \return True if it has this attribute
    bool has_attribute(const std::string &name);

    /// Remove attribute of this element by name
    /// \param name Name of the attribute to remove
    void remove_attribute(const std::string &name);

    /// Serialize this node and descendants
    /// \param tab_size Size of one tab in spaces
    /// \param level Current level of indent
    /// \return String representation of this node and descendants
    std::string serialize(size_t tab_size, size_t level) override;

    /// Returns attribute value by name
    /// \param name Name of the attribute
    /// \return Attribute value
    std::string attribute(const std::string &name);

    /// Returns concatenation of every text node descendant of this element
    /// \return Text content
    std::string text_content() override;

    /// Create new attribute
    /// \param name Name of the attribute
    /// \param value Value of the attribute
    void set_attribute(const std::string &name, const std::string &value);

    /// Delete all descendants and replace them with one Text node
    /// \param text Text node value
    void set_text_content(const std::string &text) override;

    /// Move operator=
    /// \param other Element to move
    /// \return Ref to *this
    Element &operator=(Element &&other) noexcept;

protected:
    std::map<std::string, std::string> attributes;
};

class Text : public Node
{
public:
    /// Text constructor
    /// \param text Text value
    explicit Text(const std::string &text = "");

    /// Always throws DOMError
    /// \param new_child Child to append
    void append_child(Node *new_child) override;

    /// Always throws DOMError
    /// \param new_child
    /// \param ref_child
    void insert_before(Node *new_child, Node *ref_child) override;

    /// Set new text content
    /// \param text New text
    void set_text_content(const std::string &text) override;

    /// Serialize this node and descendants
    /// \param tab_size Size of one tab in spaces
    /// \param level Current level of indent
    /// \return String representation of this node and descendants
    std::string serialize(size_t tab_size, size_t level) override;
};

class CDATASection : public Node
{
public:
    /// CDATASection constructor
    /// \param text Text content
    explicit CDATASection(const std::string &text = "");

    /// Always throws DOMError
    /// \param new_child Child to append
    void append_child(Node *new_child) override;

    /// Always throws DOMError
    /// \param new_child
    /// \param ref_child
    void insert_before(Node *new_child, Node *ref_child) override;

    /// Serialize this node and descendants
    /// \param tab_size Size of one tab in spaces
    /// \param level Current level of indent
    /// \return String representation of this node and descendants
    std::string serialize(size_t tab_size, size_t level) override;
};

class Comment : public Node
{
public:
    /// Comment constructor
    /// \param text Text content
    explicit Comment(const std::string &text = "");

    /// Always throws DOMError
    /// \param new_child Text content
    void append_child(Node *new_child) override;

    /// Always throws DOMError
    /// \param new_child
    /// \param ref_child
    void insert_before(Node *new_child, Node *ref_child) override;

    /// Serialize this node and descendants
    /// \param tab_size Size of one tab in spaces
    /// \param level Current level of indent
    /// \return String representation of this node and descendants
    std::string serialize(size_t tab_size, size_t level) override;
};

class Document : public Node
{
public:
    explicit Document();

    /// Move constructor
    /// \param other Document to move
    Document(Document&& other) noexcept;

    /// Move operator=
    /// \param other Document to move
    /// \return Ref to *this
    Document &operator=(Document&& other) noexcept;

    /// Returns XML prolog of this document
    /// \return XML prolog
    const std::string &xml_prolog() const;

    /// Sets new XML prolog
    /// \param xml_prolog New XML prolog
    void set_xml_prolog(const std::string &xml_prolog);

    /// Returns doctype of this document
    /// \return Doctype
    const std::string &doctype() const;

    /// Sets new doctype
    /// \param doctype New Doctype
    void set_doctype(const std::string &doctype);

    /// Returns pointer to root element
    /// \return Pointer to root element
    Element *root_element() const;

    /// Appends new child
    /// \param new_child Child to append
    void append_child(Node *new_child) override;

    void insert_before(Node *new_child, Node *ref_child) override;
    std::string serialize(size_t tab_size = 0);

protected:
    std::string xml_prolog_;
    std::string doctype_;
    Element* root_element_;
};

} // namespace XML::DOM

#endif //XML_DOM_HPP
