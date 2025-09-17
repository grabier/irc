# include "client.hpp"

Client::Client(void)
: _sock_fd(), _address(),
_nick(), _user(), _real_name(),
_isAuthenticated(false), _isNick_set(false),
_isUser_set(false), _isRegistered(false),
_channels(), _message_buffer()
{}

Client::Client(const Client& other) {}

Client& Client::operator=(const Client& other) {}

Client::~Client(void) {}

Client::Client(int fd, sockaddr_in address)
: _sock_fd(fd), _address(address),
_nick(), _user(), _real_name(),
_isAuthenticated(false), _isNick_set(false),
_isUser_set(false), _isRegistered(false),
_channels(), _message_buffer()
{}

void	Client::set_nick(std::string& nickname)
{
	this->_nick = nickname;
	this->_isNick_set = true;
}

void	Client::set_user(std::string& username)
{
	this->_user = username;
	this->_isUser_set = true;
}

void	Client::set_real_name(std::string& realname)
{
	this->_real_name = realname;
}

void	Client::set_authenticated_status( bool status )
{
	this->_isAuthenticated = status;
}

void	Client::set_register_status(bool status)
{
	this->_isRegistered = status;
}

bool	Client::isAuthenticated(void) const 
{
	// Must be set to true in server when handling password
	/* void Server::handlePassCommand(Client* client, const std::string& password)
	{
		if (password == this->_server_password)  // Server stores the password
    	{
        	client->set_authenticated(true);  // Mark client as authenticated
    }*/
	return (this->_isAuthenticated);
}

bool Client::isRegistered(void) const
{
	return (this->_isAuthenticated && this->_isNick_set && this->_isUser_set);
}

int	Client::get_sock_fd(void)
{
	return (this->_sock_fd);
}

std::string	Client::get_nick(void)
{
	return (this->_nick);
}

std::string	Client::get_user(void)
{
	return (this->_user);
}

std::string	Client::get_real_name(void)
{
	return (this->_real_name);
}

void	Client::addChannel(Channel* channel, const std::string& key)
{
	if (channel->canJoin(*this, key))
	{
		this->_channels.push_back(channel);
		channel->addClient(*this, key);
	}
}

void	Client::removeChannel(Channel* channel)
{
	for (std::list<Channel*>::iterator it = this->_channels.begin(); it != this->_channels.end(); it++)
	{
		if (*it == channel)
		{
			this->_channels.erase(it);
			channel->removeClient(*this);
			return ;
		}
	}
}

bool	Client::isInChannel(Channel* channel) const
{
	if (channel->hasClient(*this))
		return (true);
	return (false);
}

std::list<Channel*>	Client::getChannels(void)
{
	return (this->_channels);
}

void	Client::appendToBuffer(const std::string& data)
{
	this->_message_buffer += data;
}

std::string	Client::extractCompleteMessage(void)
{
	size_t pos = this->_message_buffer.find("\r\n");
	if (pos != std::string::npos)
	{
		std::string result = this->_message_buffer.substr(0, pos);
		this->_message_buffer.erase(0, pos + 2);
		return (result);
	}
	return ("");
}
