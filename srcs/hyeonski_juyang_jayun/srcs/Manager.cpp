#include "../includes/Manager.hpp"
#include "Location.hpp"
#include "Type.hpp"


Manager* Manager::instance;

int const Manager::decodeMimeBase64[256] = {
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 00-0F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 10-1F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,  /* 20-2F */
    52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,  /* 30-3F */
    -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,  /* 40-4F */
    15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,  /* 50-5F */
    -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,  /* 60-6F */
    41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,  /* 70-7F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 80-8F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 90-9F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* A0-AF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* B0-BF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* C0-CF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* D0-DF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* E0-EF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1   /* F0-FF */
};

Manager::Manager()
{
	this->initMimeType();
	this->initStatusCode();
}

Manager::Manager(const Manager &src)
{
	(void)src;
}

Manager &Manager::operator=(const Manager &src)
{
	(void)src;
	return (*this);
}

Manager::~Manager()
{
	delete (this->instance);
	return ;
}

Manager *Manager::getInstance()
{
	if (instance == NULL)
		instance = new Manager();
	return (instance);
}

bool	Manager::returnFalseWithMsg(const char *str)
{
	std::cerr << str << std::endl;
	return (false);
}

std::map<int, Server> &Manager::getServerConfigs()
{
	return (this->server_configs);
}

std::map<std::string, std::string> &Manager::getMimeType()
{
	return (this->mime_type);
}

std::map<std::string, std::string> &Manager::getStatusCode()
{
	return (this->status_code);
}

void	Manager::deleteFromFDTable(int fd, FDType *fd_type, t_fdset set)
{
	delete fd_type;
	MANAGER->getFDTable()[fd] = NULL;
	MANAGER->getFDTable().erase(fd);
	clrFDonTable(fd, set);
	close(fd);
}

bool	Manager::isReserved(const std::string &src)
{
	if (src == "server" || 
		src == "listen" || 
		src == "server_name" || 
		src == "location" || 
		src == "error_page" || 
		src == "allow_methods" || 
		src == "root" ||
		src == "index" ||
		src == "upload_path" ||
		src == "auto_index" ||
		src == "request_max_body_size" ||
		src == "auth_key" ||
		src == "cgi_extension" ||
		src == "return" ||
		src == "}" ||
		src == "{" )
		return (true);
	return (false);
}

bool	Manager::parseConfig(const char *config_file_path)
{
	int				fd;
	std::string		line;
	int				ret;
	std::string		splited;
	std::vector<std::string> vec;
	int key;
	std::string		location_name;

	fd = open(config_file_path, O_RDONLY);
	if (fd < 3)
		return (returnFalseWithMsg("Can't open Config file"));
	try
	{
		while ( (ret = get_next_line(fd, line)) > 0)
		{
			if (line == "")
				continue ;
			splited = ft_split(line, " \t", vec);
			line.clear();
		}

		for (std::vector<std::string>::iterator iter = vec.begin(); iter != vec.end(); iter++)
		{
			if (*iter == "server_name")
			{
				iter++;
				std::string server_name = *iter;
				iter++; // listen
				iter++; // 8080
				key = ft_atoi(*iter);
				iter++; // 127.0.0.1
				if (instance->server_configs.find(key) != instance->server_configs.end()) // 이미 존재
					throw "server_name and port already exists";
				instance->server_configs[key].setServerName(server_name);
				instance->server_configs[key].setPort(key);
				instance->server_configs[key].setIP(*iter);
			}
			else if (*iter == "location")
			{
				iter++;
				location_name = *iter;
				instance->server_configs[key].getLocations()[location_name].setLocationName(location_name);
			}
			else if (*iter == "error_page")
			{
				iter++;
				int key2 = ft_atoi(*iter);
				iter++;
				instance->server_configs[key].getLocations()[location_name].getErrorPages()[key2] = *iter;
			}
			else if (*iter == "allow_methods")
			{
				iter++;
				while (!isReserved(*iter) && iter != vec.end())
				{
					instance->server_configs[key].getLocations()[location_name].getAllowMethods().push_back(*iter);
					iter++;
				}
				if (iter == vec.end())
					break ;
				iter--;
			}
			else if (*iter == "root")
			{
				iter++;
				instance->server_configs[key].getLocations()[location_name].setRoot(*iter);
			}
			else if (*iter == "index")
			{
				iter++;
				while (!isReserved(*iter) && iter != vec.end())
				{
					instance->server_configs[key].getLocations()[location_name].getIndex().push_back(*iter);
					iter++;
				}
				if (iter == vec.end())
					break ;
				iter--;
			}
			else if (*iter == "upload_path")
			{
				iter++;
				instance->server_configs[key].getLocations()[location_name].setUploadPath(*iter);
			}
			else if (*iter == "auto_index")
			{
				iter++;
				if (*iter == "on")
					instance->server_configs[key].getLocations()[location_name].setAutoIndex(true);
				else
					instance->server_configs[key].getLocations()[location_name].setAutoIndex(false);
			}
			else if (*iter == "request_max_body_size")
			{
				iter++;
				instance->server_configs[key].getLocations()[location_name].setRequestMaxBodySize(ft_atoi(*iter));
			}
			else if (*iter == "cgi_extension")
			{
				iter++;
				instance->server_configs[key].getLocations()[location_name].getCgiExtensions().push_back(*iter);
			}
			else if (*iter == "auth_key")
			{
				iter++;
				instance->server_configs[key].getLocations()[location_name].setAuthKey(*iter);
			}
			else if (*iter == "return")
			{
				iter++;
				instance->server_configs[key].getLocations()[location_name].setRedirectReturn(ft_atoi(*iter));
				iter++;
				instance->server_configs[key].getLocations()[location_name].setRedirectAddr(*iter);				
			}
		}
	}
	catch(const char *e)
	{
		std::cout << e << std::endl;
		close(fd);
		return (false);
	}
	close(fd);
	return (true);	
}

std::map<int, FDType *> &Manager::getFDTable()
{
	return (this->fd_table);
}

Webserver &Manager::getWebserver()
{
	return (this->webserver);
}

fd_set &Manager::getReads(void)
{
	return (this->reads);
}

fd_set &Manager::getWrites(void)
{
	return (this->writes);
}

fd_set &Manager::getErrors(void)
{
	return (this->errors);
}

int Manager::decode_base64(const char * text, char * dst, int numBytes)
{
    const char* cp;
    int space_idx = 0, phase;
    int d, prev_d = 0;
    char c;
    space_idx = 0;
    phase = 0;
    for (cp = text; *cp != '\0'; ++cp) {
        d = Manager::decodeMimeBase64[(int)*cp];
        if (d != -1) {
            switch (phase) {
            case 0:
                ++phase;
                break;
            case 1:
                c = ((prev_d << 2) | ((d & 0x30) >> 4));
                if (space_idx < numBytes)
                    dst[space_idx++] = c;
                ++phase;
                break;
            case 2:
                c = (((prev_d & 0xf) << 4) | ((d & 0x3c) >> 2));
                if (space_idx < numBytes)
                    dst[space_idx++] = c;
                ++phase;
                break;
            case 3:
                c = (((prev_d & 0x03) << 6) | d);
                if (space_idx < numBytes)
                    dst[space_idx++] = c;
                phase = 0;
                break;
            }
            prev_d = d;
        }
    }
    return space_idx;
}

void	Manager::initMimeType(void)
{
	this->mime_type[".aac"] = "audio/aac";
	this->mime_type[".abw"] = "application/x-abiword";
	this->mime_type[".arc"] = "application/octet-stream";
	this->mime_type[".avi"] = "video/x-msvideo";
	this->mime_type[".azw"] = "application/vnd.amazon.ebook";
	this->mime_type[".bin"] = "application/octet-stream";
	this->mime_type[".bz"] = "application/x-bzip";
	this->mime_type[".bz2"] = "application/x-bzip2";
	this->mime_type[".csh"] = "application/x-csh";
	this->mime_type[".css"] = "text/css";
	this->mime_type[".csv"] = "text/csv";
	this->mime_type[".doc"] = "application/msword";
	this->mime_type[".epub"] = "application/epub+zip";
	this->mime_type[".gif"] = "image/gif";
	this->mime_type[".htm"] = "text/html";
	this->mime_type[".html"] = "text/html";
	this->mime_type[".ico"] = "image/x-icon";
	this->mime_type[".ics"] = "text/calendar";
	this->mime_type[".jar"] = "Temporary Redirect";
	this->mime_type[".jpeg"] = "image/jpeg";
	this->mime_type[".jpg"] = "image/jpeg";
	this->mime_type[".js"] = "application/js";
	this->mime_type[".json"] = "application/json";
	this->mime_type[".mid"] = "audio/midi";
	this->mime_type[".midi"] = "audio/midi";
	this->mime_type[".mpeg"] = "video/mpeg";
	this->mime_type[".mpkg"] = "application/vnd.apple.installer+xml";
	this->mime_type[".odp"] = "application/vnd.oasis.opendocument.presentation";
	this->mime_type[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	this->mime_type[".odt"] = "application/vnd.oasis.opendocument.text";
	this->mime_type[".oga"] = "audio/ogg";
	this->mime_type[".ogv"] = "video/ogg";
	this->mime_type[".ogx"] = "application/ogg";
	this->mime_type[".pdf"] = "application/pdf";
	this->mime_type[".ppt"] = "application/vnd.ms-powerpoint";
	this->mime_type[".rar"] = "application/x-rar-compressed";
	this->mime_type[".rtf"] = "application/rtf";
	this->mime_type[".sh"] = "application/x-sh";
	this->mime_type[".svg"] = "image/svg+xml";
	this->mime_type[".swf"] = "application/x-shockwave-flash";
	this->mime_type[".tar"] = "application/x-tar";
	this->mime_type[".tif"] = "image/tiff";
	this->mime_type[".tiff"] = "image/tiff";
	this->mime_type[".ttf"] = "application/x-font-ttf";
	this->mime_type[".vsd"] = " application/vnd.visio";
	this->mime_type[".wav"] = "audio/x-wav";
	this->mime_type[".weba"] = "audio/webm";
	this->mime_type[".webm"] = "video/webm";
	this->mime_type[".webp"] = "image/webp";
	this->mime_type[".woff"] = "application/x-font-woff";
	this->mime_type[".xhtml"] = "application/xhtml+xml";
	this->mime_type[".xls"] = "application/vnd.ms-excel";
	this->mime_type[".xml"] = "application/xml";
	this->mime_type[".xul"] = "application/vnd.mozilla.xul+xml";
	this->mime_type[".zip"] = "application/zip";
	this->mime_type[".3gp"] = "video/3gpp audio/3gpp";
	this->mime_type[".3g2"] = "video/3gpp2 audio/3gpp2";
	this->mime_type[".7z"] = "application/x-7z-compressed";
}

void	Manager::initStatusCode(void)
{
	this->status_code["100"] = "Continue";
	this->status_code["101"] = "Switching Protocols";
	this->status_code["102"] = "Processing";
	this->status_code["200"] = "OK";
	this->status_code["201"] = "Created";
	this->status_code["202"] = "Accepted";
	this->status_code["203"] = "Non-authoritative Information";
	this->status_code["204"] = "No Content";
	this->status_code["205"] = "Reset Content";
	this->status_code["206"] = "Partial Content";
	this->status_code["207"] = "Multi-Status";
	this->status_code["208"] = "Already Reported";
	this->status_code["226"] = "IM Used";
	this->status_code["300"] = "Multiple Choices";
	this->status_code["301"] = "Moved Permanently";
	this->status_code["302"] = "Found";
	this->status_code["303"] = "See Other";
	this->status_code["304"] = "Not Modified";
	this->status_code["305"] = "Use Proxy";
	this->status_code["307"] = "Temporary Redirect";
	this->status_code["308"] = "Permanent Redirect";
	this->status_code["400"] = "Bad Request";
	this->status_code["401"] = "Unauthorized";
	this->status_code["402"] = "Payment Required";
	this->status_code["403"] = "Forbidden";
	this->status_code["404"] = "Not found";
	this->status_code["405"] = "Method Not Allowed";
	this->status_code["406"] = "Not Acceptable";
	this->status_code["407"] = "Proxy Authentication Required";
	this->status_code["408"] = "Required Timeout";
	this->status_code["409"] = "Conflict";
	this->status_code["410"] = "Gone";
	this->status_code["411"] = "Length Required";
	this->status_code["412"] = "Precondition Failed";
	this->status_code["413"] = "Request Entity Too Large";
	this->status_code["414"] = "Request URI Too Long";
	this->status_code["415"] = "Unsupported Media Type";
	this->status_code["416"] = "Requested Range Not Satisfiable";
	this->status_code["417"] = "Expectation Failed";
	this->status_code["418"] = "IM_A_TEAPOT";
	this->status_code["500"] = "Internal Server Error";
	this->status_code["501"] = "Not Implemented";
	this->status_code["502"] = "Bad Gateway";
	this->status_code["503"] = "Service Unavailable";
	this->status_code["504"] = "Gateway Timeout";
	this->status_code["505"] = "HTTP Version Not Supported";
	this->status_code["506"] = "Variant Also Negotiates";
	this->status_code["507"] = "Insufficient Storage";
	this->status_code["508"] = "Loop Detected";
	this->status_code["510"] = "Not Extened";
	this->status_code["511"] = "Network Authentication Required";
	this->status_code["599"] = "Network Connect Timeout Error";
}
