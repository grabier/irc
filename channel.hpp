#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <sstream>
# include <string.h>
# include <fcntl.h>
# include <sys/socket.h>
# include <unistd.h>
# include <stdlib.h>
# include <cstdio>
# include <poll.h>
# include <list>
# include <vector>
# include <stdint.h>

class	Client;

class	Channel
{

private:
	std::string			_name;
	std::string			_topic;
	std::string			_key;
	size_t				_client_limit;

	bool				_invite_only;
	bool				_topic_restricted;
	bool				_has_key;
	bool				_has_client_limit;

	std::list<Client*>	_client_list;
	std::list<Client*>	_operators;
	std::list<Client*>	_invited_clients;

public:
	Channel( void );
	Channel( const Channel& other );
	Channel& operator=( const Channel& other );
	~Channel( void );

	Channel( const std::string& name );

	bool		addClient( Client& client, const std::string& key );
	bool		removeClient( const Client& client );
	bool		hasClient( const Client& client );
	std::string	getName( void ) const;
	size_t		getClientCount( void ) const;
	bool		addOperator( Client& client );
	bool		removeOperator( const Client& client );
	bool		removeInvitedClient( const Client& client );
	bool		isOperator( const Client& client );
	bool		isInvitedClient( const Client& client );
	bool		setTopic( std::string& topic, Client& client );
	std::string	getTopic( void ) const;
	bool		kickClient( const Client& kicker, Client& target );
	bool		inviteClient( Client & inviter, Client& client );
	bool		setMode( char mode, std::string param, Client& requester, Client& target );
	bool		removeMode( char mode, Client& requester, Client& target );
	bool		validateKey( const std::string& key );
	bool		isInviteOnly( void );
	bool		isTopicRestricted( void );
	bool		broadcastMessage( const std::string message );
	bool		canJoin( Client& client, const std::string& key );

};

#endif
