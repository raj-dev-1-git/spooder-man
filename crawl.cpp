#include <bits/stdc++.h>
#include <unordered_map>
#include <curl/curl.h>
#include <regex>
#include <thread>

#include <queue>
using namespace std;



// Function to download HTML content and save it to a file
void get_page(const string& url, const string& file_name)
{
    CURL* easyhandle = curl_easy_init();
    if (!easyhandle) return;
    curl_easy_setopt(easyhandle, CURLOPT_URL, url.c_str());
    FILE* file = fopen(file_name.c_str(), "w");
    if (!file) {
        curl_easy_cleanup(easyhandle);
        return;
    }
    curl_easy_setopt(easyhandle, CURLOPT_WRITEDATA, file);
    curl_easy_perform(easyhandle);
    curl_easy_cleanup(easyhandle);
    fclose(file);
}

// Function to extract hyperlinks from a file
vector<string> extract_hyperlinks(const string& html_file_name)
{
    ifstream read(html_file_name);
    if (!read.is_open()) return {};

    string html((istreambuf_iterator<char>(read)), istreambuf_iterator<char>());
    read.close();

    static const regex hl_regex("<a href=\"(.*?)\">", regex_constants::icase);
    vector<string> links;
    copy(sregex_token_iterator(html.begin(), html.end(), hl_regex, 1), sregex_token_iterator(), back_inserter(links));
    return links;
}

// Function to clean and validate hyperlinks
vector<string> clean_links(const vector<string>& all_links)
{
    vector<string> final_links;
    static const regex url_regex("((http|https)://)(www\\.)?[a-zA-Z0-9@:%._\\+~#?&//=]{2,256}\\.[a-z]{2,6}\\b([-a-zA-Z0-9@:%._\\+~#?&//=]*)");
    for (const string& link : all_links)
    {
        string cleaned_link = link.substr(0, link.find_first_of(" \""));
        if (regex_match(cleaned_link, url_regex))
        {
            final_links.push_back(cleaned_link);
        }
    }
    return final_links;
}

unordered_map<string, bool> visited;

// BFS Web Crawler function
void bfs_crawler(const string& start_url, const string& file_path, int max_depth)
{
    queue<pair<string, int>> q;
    q.push({start_url, 0});
    visited[start_url] = true;

    while (!q.empty())
    {
        auto [url, depth] = q.front();
        q.pop();

        if (depth >= max_depth) continue;

        {
            
            cout << "Depth: " << depth << "\tLink: " << url << endl;
        }

        get_page(url, file_path);
        vector<string> allLinksData = extract_hyperlinks(file_path);
        vector<string> cleanedLinks = clean_links(allLinksData);

        for (const string& link : cleanedLinks)
        {
            if (!visited[link])
            {
                visited[link] = true;
                q.push({link, depth + 1});
            }
        }
    }
}

int main()
{
    string start_url;
    cout << "Enter the starting URL: ";
    cin >> start_url;

    const string filename = "html.txt";
    bfs_crawler(start_url, filename, 10);
   

    return 0;
}
