#include <cstdio>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

using namespace std;

class HRML
{
public:

    HRML(istream& input, ostream& output, int nlines, int nqueries);
    void RunQueries();

private:

    struct field
    {
        string tag;
        vector<pair<string, string>> attributes;

        field() : tag("") {};
    };

    void ParseLines(int nlines);
    void ParseQueries(int nqueries);

    istream& m_input;
    ostream& m_output;
    vector<field> m_fields;
    vector<string> m_queries;
};

HRML::HRML(istream& input, ostream& output, int nlines, int nqueries) : m_input(input), m_output(output)
{
    ParseLines(nlines);
    ParseQueries(nqueries);
}

void HRML::RunQueries()
{
    bool found;
    auto match = smatch{};
    regex query { R"((.*)~(.*))" };

    for(auto i : m_queries)
    {
        found = false;

        if(regex_match(i, match, query))
        {
            for(auto j : m_fields)
            {
                if(match[1].str() == j.tag)
                {
                    for(auto k : j.attributes)
                    {
                        if(k.first == match[2].str())
                        {
                            found = true;
                            m_output << k.second << endl;
                            break;
                        }
                    }
                    break;
                }
            }
        }

        if(!found) { cout << "Not Found!" << endl; }
    }
}

void HRML::ParseLines(int nlines)
{
    HRML::field tmpfield;
    string sline, key, value;
    stringstream keys_and_values;
    char equals_sign;

    auto match = smatch {};
    regex opening_line { R"(<(\w+)\s*(.*)>)" };
    regex closing_line { R"(<\/(\w+).*>)" };

    vector<string> opentags;

    while(nlines)
    {
        getline(m_input, sline);

        if(regex_match(sline, match, opening_line))
        {
            if(opentags.empty())
            {
                tmpfield.tag = match[1].str();
                opentags.push_back(match[1].str());
            }
            else
            {
                opentags.push_back(match[1].str());
                tmpfield.tag.erase();
                for(auto i : opentags)
                {
                    if(tmpfield.tag.empty())    { tmpfield.tag =  i; }
                    else                        { tmpfield.tag =  tmpfield.tag + "." + i; }
                }
            }

            tmpfield.attributes.clear();
            keys_and_values.clear();

            keys_and_values << match[2].str();

            // White spaces are needed around the equals sign
            while(keys_and_values >> key >> equals_sign >> value)
            {
                value.erase(remove(value.begin(), value.end(), '"'), value.end());
                tmpfield.attributes.push_back(pair<string, string>(key, value));
            }

            m_fields.push_back(tmpfield);
        }

        if(regex_match(sline, match, closing_line))
        {
            if(match[1].str() == opentags.back())
            {
                opentags.pop_back();
            }
            else
            {
                m_fields.clear();                           // Invalid input
                break;
            }
        }

        --nlines;
    }

    if(!opentags.empty())   { m_fields.clear(); }           // Invalid input

};

void HRML::ParseQueries(int nqueries)
{
    vector<string> queries;
    string squery;

    while(nqueries)
    {
        getline(m_input, squery);
        m_queries.push_back(squery);
        --nqueries;
    }

}

int main() {
    int nlines, nqueries;

    cin >> nlines >> nqueries;
    cin.ignore(1, '\n');                                    // throw away the \n at the line end

    HRML fieldsAndQueries(cin, cout, nlines, nqueries);
    fieldsAndQueries.RunQueries();

    return 0;
}
