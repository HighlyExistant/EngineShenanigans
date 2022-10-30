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

        void print();
    };

    struct Element
    {
        std::string nmsp;   // Namespace
        std::string element;
        std::vector<Attribute> attributes;
        std::vector<Element> children;

        void print_recursive();
    };

    char* XmlWriter(Element element, size_t* size);

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

    class XmlParser : protected StringParser
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
        std::string getStrSplit();
        std::vector<Element> parse();
        Attribute parseAttribute();
        bool checkEnding(std::string nmsp, std::string elementName);
        Element parseElement();
    };
} // namespace cow