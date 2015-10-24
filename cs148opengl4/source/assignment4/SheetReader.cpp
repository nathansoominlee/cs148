#include "assignment4/SheetReader.h"

int SheetReader::ImportSheet(std::string sheet, // input parameter
                             Rows &rows)        // output parameter
{
    std::string fname;

    if (sheet.substr(0, 5) == "https")
    {
        fname = DownloadSheet(sheet);
        if (fname == "")
        {
            std::cerr << "Error: Failed downloading sheet " + sheet + ". Returning nullptr" << std::endl;
            return -1;
        }
    }
    else if(sheet.substr(sheet.length() - 4) == ".tsv")
    {
        fname = sheet;
    }
    else
    {
        std::cerr << "Error: Expected https URL or .tsv file for sheet. Returning nullptr" << std::endl;
        return -1;
    }

    rows = ParseTSV(fname);
    
    return 0; 

}

// Reads TSV line by line and extracts fields
Rows SheetReader::ParseTSV(std::string fname)
{
    Rows rows;

    std::ifstream file(fname);
    std::string line;

    const char *needle = "\t";

    while (std::getline(file, line))
    {
        rows.push_back(Rows::value_type());

        const char *haystack = line.c_str();
        char *field;

        while ( (field = strnstr(haystack, needle, line.length())) )
        {
            // put in null char
            (*field) = '\0';

            rows.back().push_back(haystack);

            haystack = field + 1;
        }

        // Collect the last field
        rows.back().push_back(haystack);

    }

    return std::move(rows);
}


std::string SheetReader::DownloadSheet(std::string sheet)
{
    // check for curl
    std::string cmd = "curl --version > /dev/null 2>&1";
    int retval = system(cmd.c_str());
    if (retval)
    {
        std::cerr << "Error: curl is missing, " + cmd + " returned non-zero exit code " 
            + std::to_string(retval)<< std::endl;
        return "";
    }

    // download sheet
    std::string fname = "final_scene_sheet.tsv";
    cmd = "curl -sk " + sheet + " > " + fname;

    retval = system(cmd.c_str());

    if (retval)
    {
        std::cerr << "Error: " + cmd + " returned non-zero exit code " + std::to_string(retval) << std::endl;
        return "";
    }

    return fname;
}
