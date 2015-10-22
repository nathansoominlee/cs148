#include "assignment4/SheetReader.h"

int SheetReader::ImportSheet(std::string sheet, std::vector<std::vector<std::string>> &rows)
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
    else if(sheet.substr(sheet.length() - 4 - 1) != ".tsv")
    {
        std::cerr << "Error: Expected https URL or .tsv file for sheet. Returning nullptr" << std::endl;
        return -1;
    }

    rows = ParseTSV(fname);
    
    return 0; 

}

// Copied from http://stackoverflow.com/questions/1075712/reading-delimited-files-in-c
std::vector<std::vector<std::string>> SheetReader::ParseTSV(std::string fname)
{
    std::vector<std::vector<std::string>> rows;

    std::ifstream input(fname);

    char const row_delim = '\n';
    char const field_delim = '\t';

    for (std::string row; getline(input, row, row_delim); /* no increment */ ) 
    {
        rows.push_back(std::vector<std::string>());

        std::istringstream ss(row);

        for (std::string field; getline(ss, field, field_delim); /* no increment */ ) 
        {
            rows.back().push_back(field);
        }

    }

    return std::move(rows);
}


std::string SheetReader::DownloadSheet(std::string sheet)
{
    // check for curl
    std::string cmd = "curl --version";
    int retval = system(cmd.c_str());
    if (retval)
    {
        std::cerr << "Error: curl is missing, " + cmd + " returned non-zero exit code " 
            + std::to_string(retval)<< std::endl;
        return "";
    }

    // download sheet
    std::string fname = "final_scene_sheet.tsv";
    cmd = "curl -k " + sheet + " > " + fname;

    retval = system(cmd.c_str());

    if (retval)
    {
        std::cerr << "Error: " + cmd + " returned non-zero exit code " + std::to_string(retval) << std::endl;
        return "";
    }

    return fname;
}
