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

    std::string tab = "\t";

    while (std::getline(file, line))
    {
        rows.push_back(Rows::value_type());
        
        int tab_pos = -1; 
        int last_tab_pos = -1;

        do
        {
            last_tab_pos = tab_pos + 1;
            tab_pos = line.find(tab, tab_pos + 1);

            std::size_t start = last_tab_pos;
            std::size_t count = tab_pos - last_tab_pos;
            std::string field = line.substr(start, count);

            rows.back().push_back(field);
        }
        while (tab_pos != std::string::npos);

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
