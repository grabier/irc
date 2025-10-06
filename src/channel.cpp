/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ppeckham <ppeckham@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 13:08:29 by ppeckham          #+#    #+#             */
/*   Updated: 2025/10/06 16:45:28 by ppeckham         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "channel.hpp"
# include "client.hpp"

Channel::Channel(void)
:  _name(), _topic(), _key(), _client_limit(100),
_invite_only(false), _topic_restricted(false),
_has_key(false), _has_client_limit(false),
_client_list(), _operators(), _invited_clients()
{}

Channel::Channel(const Channel& other)
{
	this->_name = other._name;
	this->_topic = other._topic;
	this->_key = other._key;
	this->_client_limit = other._client_limit;
	this->_invite_only = other._invite_only;
	this->_topic_restricted = other._topic_restricted;
	this->_has_key = other._has_key;
	this->_has_client_limit = other._has_client_limit;
	this->_client_list = other._client_list;
	this->_operators = other._operators;
	this->_invited_clients = other._invited_clients;

}

Channel& Channel::operator=(const Channel& other)
{
	if (this != &other)
	{
		this->_name = other._name;
		this->_topic = other._topic;
		this->_key = other._key;
		this->_client_limit = other._client_limit;
		this->_invite_only = other._invite_only;
		this->_topic_restricted = other._topic_restricted;
		this->_has_key = other._has_key;
		this->_has_client_limit = other._has_client_limit;
		this->_client_list = other._client_list;
		this->_operators = other._operators;
		this->_invited_clients = other._invited_clients;
	}
	return (*this);
}

Channel::~Channel(void) {} // check is anything must be deleted

Channel::Channel(const std::string& name) 
:  _name(name), _topic(), _key(), _client_limit(100),
_invite_only(false), _topic_restricted(false),
_has_key(false), _has_client_limit(false),
_client_list(), _operators(), _invited_clients()
{}

bool	Channel::addClient(Client& client, const std::string& key)
{
	if (canJoin(client, key))
	{
		this->_client_list.push_back(&client);
		return (true);
	}
	return (false);
}

bool	Channel::removeClient(const Client& client)
{
	for (std::list<Client*>::iterator it = this->_client_list.begin();
		it != this->_client_list.end(); it++)
	{
		if (&client == *it)
		{
			if (isOperator(client))
				removeOperator(client);
			if (isInvitedClient(client))
				removeInvitedClient(client);
			this->_client_list.erase(it);
			return (true);
		}
	}
	return (false);
}

bool	Channel::hasClient(const Client& client) // checks if client is un channel
{
	for (std::list<Client*>::iterator it = this->_client_list.begin();
		it != this->_client_list.end(); it++)
		if (&client == *it)
			return (true);
	return (false);
}

std::string	Channel::getName(void) const
{
	return (this->_name);
}

size_t	Channel::getClientCount(void) const
{
	return (this->_client_list.size());
}

size_t	Channel::getOperatorCount(void) const
{
	return (this->_operators.size());
}

bool	Channel::addOperator(Client& client)
{
	if (hasClient(client))
	{
		for (std::list<Client*>::iterator it = this->_client_list.begin();
			it != this->_client_list.end(); it++)
		{
			if (&client == *it)
			{
				this->_operators.push_back(*it);
				return (true);
			}
		}
	}
	return (false);
}

bool	Channel::removeOperator(const Client& client)
{
	for (std::list<Client*>::iterator it = this->_operators.begin();
		it != this->_operators.end(); it++)
	{
		if (&client == *it)
		{
			this->_operators.erase(it);
			return (true);
		}
	}
	return (false);
}

void	Channel::addInvitedClient(Client& client)
{
	this->_invited_clients.push_back(&client);
}

bool	Channel::removeInvitedClient(const Client& client)
{
	for (std::list<Client*>::iterator it = this->_invited_clients.begin();
		it != this->_invited_clients.end(); it++)
	{
		if (&client == *it)
		{
			this->_invited_clients.erase(it);
			return (true);
		}
	}
	return (false);
}

bool	Channel::isOperator(const Client& client)
{
	for (std::list<Client*>::iterator it = this->_operators.begin();
		it != this->_operators.end(); it++)
		if (&client == *it)
			return (true);
	return (false);
}

bool	Channel::isInvitedClient(const Client& client)
{
	for (std::list<Client*>::iterator it = this->_invited_clients.begin();
		it != this->_invited_clients.end(); it++)
		if (&client == *it)
			return (true);
	return (false);
}

bool	Channel::setTopic(std::string& topic, Client& client)
{
	if (!this->_topic_restricted || isOperator(client))
	{
		this->_topic = topic;
		return (true);
	}
	return (false);
}

std::string	Channel::getTopic(void) const
{
	return (this->_topic);
}

bool	Channel::kickClient(const Client& kicker, Client& target)
{
	if (!isOperator(kicker))
		return (false);
	if (isOperator(target))
		removeOperator(target);
	if (isInvitedClient(target))
		removeInvitedClient(target);
	if (removeClient(target))
	{
		broadcastMessage(target.get_user() + " has been kicked out!");
		return (true);
	}
	return (false);
}

bool	Channel::inviteClient(Client&inviter, Client& client)
{
	if (!isOperator(inviter) || hasClient(client))
		return (false);
	if (isInvitedClient(client))
		return (false);
	this->addInvitedClient(client);
	return (true);
}

bool	Channel::setMode(char mode, std::string param, Client& requester, Client& target)
{
	if (!isOperator(requester))
		return (std::cout << "cliente no es operator\n", false);
	if (mode == 'i')
	{
		(void)param;
		(void)requester;
		(void)target;
		this->_invite_only = true;
		return (true);
	}
	if (mode == 't')
	{
		(void)param;
		(void)requester;
		(void)target;
		this->_topic_restricted = true;
		return (true);
	}
	if (mode == 'k')
	{
		(void)requester;
		(void)target;
		this->_has_key = true;
		this->_key = param;
		return (true);
	}
	if (mode == 'o')
	{
		(void)param;
		if (!addOperator(target))
			return (false);
		return (true);
	}
	if (mode == 'l')
	{
		for (int i = 0; param[i]; i++)
			if (!isdigit(param[i]))
				return (false);
		(void)requester;
		(void)target;
		this->_has_client_limit = true;
		std::istringstream s(param);
		int number;
		s >> number;
		if (number > 0 && number < 10000)
			this->_client_limit = number;
		else
			return (false);
		return (true);
	}
	return (false);
}

bool	Channel::removeMode(char mode, Client& requester, Client& target)
{
	if (!isOperator(requester))
		return (false);
	if (mode == 'i')
	{
		(void)requester;
		(void)target;
		this->_invite_only = false;
		return (true);
	}
	if (mode == 't')
	{
		(void)requester;
		(void)target;
		this->_topic_restricted = false;
		return (true);
	}
	if (mode == 'k')
	{
		(void)requester;
		(void)target;
		this->_has_key = false;
		this->_key = "";
		return (true);
	}
	if (mode == 'o')
	{
		removeOperator(target);
		return (true);
	}
	if (mode == 'l')
	{
		(void)requester;
		(void)target;
		this->_has_client_limit = false;
		return (true);
	}
	return (false);
}

bool	Channel::validateKey(const std::string& key)
{
	if (!this->_has_key)
		return (true);
	return (key == this->_key);
}

bool	Channel::isInviteOnly(void)
{
	if (this->_invite_only)
		return (true);
	return (false);
}

bool	Channel::isTopicRestricted(void)
{
	if (this->_topic_restricted)
		return (true);
	return (false);
}

bool	Channel::isFull(void)
{
	if (this->_has_client_limit && this->_client_limit <= this->_client_list.size())
		return (true);
	return (false);
}

bool	Channel::broadcastMessage(const std::string message)
{
	if (_client_list.empty())
		return false;

	// Add IRC protocol line ending if not present
	std::string formatted_message = message;
	if (formatted_message.length() < 2 || 
		formatted_message.substr(formatted_message.length() - 2) != "\r\n")
	{
		formatted_message += "\r\n";
	}

	for (std::list<Client*>::iterator it = _client_list.begin(); 
			it != _client_list.end(); ++it)
	{
		Client* client = *it;
		ssize_t bytes_sent = send(client->get_sock_fd(), 
									formatted_message.c_str(), 
									formatted_message.length(), 
									MSG_NOSIGNAL);
	
		if (bytes_sent == -1)
		{
			// Log error or handle disconnected client
			std::cout << "bytes_sent == -1\n";
			continue;
		}
	}
	return true;
}

bool	Channel::canJoin(Client& client, const std::string& key)
{
	if (hasClient(client) || !client.isRegistered())
		return (false);
	if (this->_invite_only && !isInvitedClient(client))
		return (false);
	if (isFull())
		return (false);
	if (!validateKey(key))
		return (false);
	return (true);
}

std::list<Client*>	Channel::getClientList(void) const
{
	return (this->_client_list);
}
