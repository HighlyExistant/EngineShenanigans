#include "xml_parser.hpp"

namespace cow
{

    void Element::print_recursive()
    {
        std::cout << "Namespace: " << nmsp << '\n';
        std::cout << "Element: " << element << '\n';
        for (size_t i = 0; i < attributes.size(); i++)
        {
            std::cout << i << ":\n";
            attributes.data()[i].print();
        }
        for (size_t i = 0; i < children.size(); i++)
        {
            children.data()[i].print_recursive();
        }
    }

    void Attribute::print()
    {
        std::cout << "Attribute Name " << ": " << name << '\n';
        std::cout << "Attribute Value " << ": " << value << '\n';
    }

    size_t find_xml_size(Element element)
    {
        /*
        * find the size of current element and check
        * for children if there is children run this
        * function recursively for each child
        */
        size_t size = 7;    // <:></>   '\0'
        size += element.nmsp.size() + (2 * !element.children.empty());
        size += element.element.size();
        for (size_t i = 0; i < element.attributes.size(); i++)
        {
            size += element.attributes.data()[i].name.size();
            size += element.attributes.data()[i].value.size() + 4;  // + 4 for whitespace, '=' and double quotes
        }
        for (size_t i = 0; i < element.children.size(); i++)
        {
            size += find_xml_size(element.children.data()[i]);
        }
        return size;
    }
    void write_xml_element(Element element, char* buf, size_t* offset)
    {
        if (buf == nullptr) return;
        bool self_closed = element.children.empty();

        if (!element.nmsp.empty())
        {
            size_t val = element.nmsp.size() + element.element.size() + 3;
            memcpy(&buf[*offset], std::string("<" + element.nmsp + ":" + element.element + " ").c_str(), val);
            *offset += val;
        }
        else
        {
            size_t val = (element.element.size() + 2);
            memcpy(&buf[*offset], std::string("<" + element.element + " ").c_str(), val);
            *offset += val;
        }
        for (size_t i = 0; i < element.attributes.size(); i++)
        {
            size_t val = element.attributes.data()[i].name.size() + element.attributes.data()[i].value.size() + 4;
            memcpy(&buf[*offset], std::string(element.attributes.data()[i].name + "=\"" + element.attributes.data()[i].value + "\" ").c_str(), val);
            *offset += val;
        }
        if (self_closed)
        {
            memcpy(&buf[*offset], "/>", 2); // null character is not picked up
            *offset += 2;
        }
        else
        {
            memcpy(&buf[*offset], ">", 1); // null character is not picked up
            *offset += 1;
            for (size_t i = 0; i < element.children.size(); i++)
            {
                write_xml_element(element.children.data()[i], buf, offset);
            }
            size_t val = element.element.size() + 3;
            memcpy(&buf[*offset], std::string("</" + element.element + ">").c_str(), val);
            *offset += val;
        }
    }
    // return value must be freed
    char* XmlWriter(Element element, size_t* size)
    {
        *size = find_xml_size(element);
        size_t offset = 0;
        char* val = new char[*size];
        if (val != nullptr)
        {
            write_xml_element(element, val, &offset);
            val[offset] = '\0';
        }
        return val;
    }
    void XmlWriter(Element element, char* buf, size_t* size)
    {
        if (size != nullptr)
            *size = find_xml_size(element);
        size_t offset = 0;
        write_xml_element(element, buf, &offset);
        buf[offset] = '\0';
    }
    // ---

    std::string XmlParser::getStrSplit()
    {
        std::string r_str;
        while (m_str[m_eye] != '\0' && !whitespace() && character() && m_str[m_eye] != ':')
        {
            r_str.push_back(m_str[m_eye]);
            advance();
        }
        return r_str;
    }
    std::vector<Element> XmlParser::parse()
    {
        removeWhitespace();
        std::vector<Element> parentElement;
        switch (m_str[m_eye])
        {
        case '<':
            advance();
            parentElement.push_back(parseElement());
            break;

        default:
            break;
        }
        return parentElement;
    }
    Attribute XmlParser::parseAttribute()
    {
        Attribute attr;
        std::string str_container = getStrSplit();
        attr.name = str_container;
        str_container.clear();
        removeWhitespace();
        if (eye() == '=')
        {
            advance();  // Currently eye is in character '=' so we advance
            removeWhitespace(); // next value has the possibility of being whitespace so we remove it
            if (eye() == '\"')
            {
                advance();
                while (m_str[m_eye] != '\"')
                {
                    str_container.push_back(m_str[m_eye]);
                    advance();
                }
                attr.value = str_container;
                advance();
                removeWhitespace();
            }
            else
            {
                throw std::runtime_error(std::string("Attribute: " + attr.name + " value signature is incorrectly defined").c_str());
            }

        }
        else
        {
            throw std::runtime_error(std::string("Attribute: " + attr.name + " holds no value").c_str());
        }

        return attr;
    }
    bool XmlParser::checkEnding(std::string nmsp, std::string elementName)
    {
        if (nmsp.empty())
        {
            removeWhitespace();
            if (checkStr("</"))
            {
                removeWhitespace();
                if (!checkStr(elementName.c_str()))
                    throw std::runtime_error("Does not close");

                if (checkCharacter('>'))
                {
                    return true;
                }
            }
        }
        else
        {
            removeWhitespace();
            if (checkStr("</"))
            {
                if (!checkStr((nmsp + ":" + elementName).c_str()))
                    throw std::runtime_error("Does not close");

                if (checkCharacter('>'))
                {
                    return true;
                }
            }
        }
        return false;
    }
    Element XmlParser::parseElement()
    {
        if (eye() == '<') advance();
        bool selfclosed = false;
        Element element;

        std::string str_container = getStrSplit();   // Recieving element name

        if (eye() == ':')
        {
            element.nmsp = str_container;
            str_container.clear();
            advance();

            str_container = getStrSplit();
            element.element = str_container;
        }
        else
        {
            element.element = str_container;
        }   // Recieving Namespace and Element
        removeWhitespace(); // Remove Whitespace

        while (eye() != '>')
        {
            if (eye() == '/')
            {
                selfclosed = true;
                advance();
                continue;
            }
            removeWhitespace(); // Seperate Attributes
            element.attributes.push_back(parseAttribute());
            removeWhitespace(); // get rid of any other whitespace
        }   // Getting attributes
        if (selfclosed)
            return element;
        advance();  // Either self close and leave early or are currently in whitespace
        for (size_t i = 0; !checkEnding(element.nmsp, element.element); i++)
        {
            removeWhitespace();
            element.children.push_back(parseElement());
            advance();
        }
        return element;
    }

}
