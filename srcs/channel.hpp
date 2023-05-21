#ifndef _CHANNEL
# define _CHANNEL

#include <iostream>
#include <vector>
#include <string>

class Channel {

    public:
        Channel( void );
        ~Channel( void );
        Channel( const Channel& other );

        int JoinClient( int client_sd, bool oprtor );
        bool PartClient( int client_sd ); //return false if not found
        void SetChannelConcept( std::string new_concept );

        std::string GetChannelConcept( void );
        std::vector<int> GetClients( void );
        std::vector<int> GetOperators( void );
        std::vector<int> GetNoOperatorClients( void );
        bool GetIfClientOperator( int client_sd );
        bool GetIfClientInChannel( int client_sd );
        bool CheckKey( std::string trying_key );
    private:
        std::string         channel_concept;
        std::vector<int>    client_sd_list;
        std::vector<int>    oprtor_sd_list;

        std::string         channel_key;

        //permisos
        bool                private_channel;
        bool                secret_channel;
        bool                invite_only;
        bool                topic_opr_only;
        bool                out_msg_reject;
        bool                moderated_channel;
        int                 channel_max_users;
};

#endif
