#include "../includes/Server.hpp"
#include "Manager.hpp"
#include "Location.hpp"
#include "Client.hpp"
#include "CGI.hpp"

Server::Server() : port(-1), socket_fd(-1)
{
	
}

Server::Server(const Server& src)
{
	this->ip = src.ip;
	this->port = src.port;
	this->server_name =	src.server_name;
	this->socket_fd = src.socket_fd;
	this->locations.insert(src.locations.begin(), src.locations.end());
	this->clients.insert(src.clients.begin(), src.clients.end());
}

Server &Server::operator=(const Server &src)
{
	this->ip = src.ip;
	this->port	=	src.port;
	this->server_name	=	src.server_name;
	this->socket_fd		=	src.socket_fd;
	this->locations.clear();
	this->locations.insert(src.locations.begin(), src.locations.end());
	this->clients.insert(src.clients.begin(), src.clients.end());

	return (*this);
}

Server::~Server()
{
	return ;
}

void	Server::setPort(unsigned short port)
{
	this->port = port;
	return ;
}

void	Server::setIP(const std::string &ip)
{
	this->ip = ip;
	return ;
}

void	Server::setServerName(const std::string &server_name)
{
	this->server_name = server_name;
	return ;
}

void	Server::setSocketFd(int socket_fd)
{
	this->socket_fd = socket_fd;
	return ;
}

const std::string &Server::getIP() const
{
	return (this->ip);
}

const std::string &Server::getServerName() const
{
	return (this->server_name);
}

unsigned short		Server::getPort() const
{
	return (this->port);
}

int					Server::getSocketFd() const
{
	return (this->socket_fd);
}

std::map<int, Client> &Server::getClients()
{
	return (this->clients);
}


std::map<std::string, Location> &Server::getLocations()
{
	return (this->locations);
}

int Server::acceptClient(int server_fd, int &fd_max)
{
	struct sockaddr_in  client_addr;
	socklen_t			addr_size = sizeof(client_addr);

	std::cout << "\033[32m server connection called \033[0m" << std::endl;	
	int client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &addr_size);
	fcntl(client_socket, F_SETFL, O_NONBLOCK);

	if (fd_max < client_socket)
		fd_max = client_socket;

	this->clients[client_socket].setServerSocketFd(server_fd);
	this->clients[client_socket].setSocketFd(client_socket);
	this->clients[client_socket].setLastRequestMs(ft_get_time());
	this->clients[client_socket].setStatus(REQUEST_RECEIVING);
	this->clients[client_socket].setServer(*this);

	//fd_table 세팅
	FDType *client_fdtype = new ClientFD(CLIENT_FDTYPE, &this->clients[client_socket]);
	MANAGER->getFDTable().insert(std::pair<int, FDType*>(client_socket, client_fdtype));

	std::cout << "connected client : " << client_socket << std::endl;
	return (client_socket);
}

bool Server::isCgiRequest(Location &location, Request &request)
{
	std::vector<std::string> &cgi_extensions = location.getCgiExtensions();

	size_t dot_pos = request.getUri().find('.');
	if (dot_pos == std::string::npos)
		return (false);
	size_t ext_end = dot_pos;
	while (ext_end != request.getUri().length() && request.getUri()[ext_end] != '/' && request.getUri()[ext_end] != '?')
		ext_end++;
	
	std::string res = request.getUri().substr(dot_pos, ext_end - dot_pos);
	if (std::find(cgi_extensions.begin(), cgi_extensions.end(), res) == cgi_extensions.end())
		return (false);
	
	while (request.getUri()[dot_pos] != '/')
		dot_pos--;
	res = request.getUri().substr(dot_pos + 1, ext_end - dot_pos - 1);


	CGI	cgi;
	cgi.testCGICall(request, location, res);
	return (true);
}

int    Server::createFileWithDirectory(std::string path)
{
    size_t  pos;
    size_t  n;
    int     fd;

    n = 0;
    pos = path.find("/", n);
    while (pos != std::string::npos)
    {
        std::string temp = path.substr(0, pos);
        mkdir(temp.c_str(), 0777);
        n = pos + 1;
        pos = path.find("/", n);
    }
    fd = open(path.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0777);
	return (fd);
}

bool Server::isCorrectAuth(Location &location, Client &client)
{
	char auth_key[200];

	memset(auth_key, 0, 200);
	std::size_t found = client.getRequest().getHeaders()["Authorization"].find(' ');
	MANAGER->decode_base64(client.getRequest().getHeaders()["Authorization"].substr(found + 1).c_str(), auth_key, client.getRequest().getHeaders()["Authorization"].length());
	if (std::string(auth_key) != location.getAuthKey())
		return (false);
	return (true);
}
