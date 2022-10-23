#pragma once
#include <iostream>
#include <vector>
#include <string>

namespace cow
{
    struct Attribute
    {
        std::string name;
        std::string value;

        void print() 
        {
            std::cout << "Attribute Name " << ": " << name << '\n';
            std::cout << "Attribute Value " << ": " << value << '\n';
        }
    };

    struct Element
    {
        std::string nmsp;   // Namespace
        std::string element;
        std::vector<Attribute> attributes;
        std::vector<Element> children;

        void print_recursive() 
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
    };

    class StringParser
    {
    protected:
        char* m_str;
        uint32_t m_eye = 0;
        uint32_t m_peek = 1;
    public:
        StringParser(char* str)
            : m_str{ str } {}
        StringParser(const char* str) : m_str{ const_cast<char*>(str) } {}
        ~StringParser() {}
        void advance(uint32_t i)
        {
            m_eye += i;
            m_peek += i;
        }
        void advance()
        {
            m_eye++;
            m_peek++;
        }
        int parseNumber() 
        {
            std::string _str;
            while (this->number())
            {
                _str.push_back(this->character());
                advance();
            }
            advance();
            
            return atoi(_str.c_str());
        }
        static bool whitespace(char c)
        {
            return c == 0x20
                || c == 0xA
                || c == 0xD
                || c == 0x9;
        }
        bool whitespace()
        {
            return m_str[m_eye] == 0x20
                || m_str[m_eye] == 0xA
                || m_str[m_eye] == 0xD
                || m_str[m_eye] == 0x9;
        }
        static bool number(char c)
        {
            return c > 48 && c < 57;
        }
        bool number()
        {
            return m_str[m_eye] >= 48 && m_str[m_eye] <= 57;
        }
        bool character()
        {
            return (m_str[m_eye] >= 65 && m_str[m_eye] <= 132)
                || (m_str[m_eye] >= 97 && m_str[m_eye] <= 122);
        }
        bool checkStr(const char* s)
        {
            for (size_t i = 0; s[i] != '\0'; i++)
            {
                if (m_str[m_eye] != s[i]) return false;
                advance();
            }
            return true;
        }
        inline bool checkCharacter(char c)
        {
            return m_str[m_eye] == c;
        }
        std::string getStr()
        {
            std::string r_str;
            for (size_t i = 0; m_str[i] != '\0'; i++)
            {
                r_str.push_back(m_str[i]);
            }
            return r_str;
        }
        inline char eye()
            const
        {
            return m_str[m_eye];
        }
        inline char peek()
            const
        {
            return m_str[m_peek];
        }
    };

    class XmlParser : public StringParser
    {
    private:
        void removeWhitespace()
        {
            while (whitespace())
                advance();
        }
    public:
        XmlParser(char* str) : StringParser{ str } {}

        ~XmlParser() {}
        std::string getStrSplit()
        {
            std::string r_str;
            while (m_str[m_eye] != '\0' && !whitespace() && character() && m_str[m_eye] != ':')
            {
                r_str.push_back(m_str[m_eye]);
                advance();
            }
            return r_str;
        }
        std::vector<Element> parse()
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
        Attribute parseAttribute()
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
        bool checkEnding(std::string nmsp, std::string elementName)
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
        Element parseElement()
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

            while( eye() != '>')
            {
                if (eye() == '/')
                {
                    selfclosed = true;
                    advance();
                    continue;
                }
                removeWhitespace(); // Seperate Attributes
                element.attributes.push_back(parseAttribute());
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
    };
} // namespace cow