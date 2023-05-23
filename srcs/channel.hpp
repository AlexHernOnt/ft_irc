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
        void MakeOperator( int client_sd );
        void UnmakeOperator( int client_sd );
        void SetChannelConcept( std::string new_concept );

        std::string GetChannelConcept( void );
        std::vector<int> GetClients( void );
        std::vector<int> GetOperators( void );
        std::vector<int> GetNoOperatorClients( void );
        bool GetIfClientOperator( int client_sd );
        bool GetIfClientInChannel( int client_sd );
        bool CheckKey( std::string trying_key );
        void SetKey( std::string new_key );

        bool GetIfNickBanned( std::string nick );
        std::vector<std::string> GetBanMasks( void );
        void SetBanMask( std::string mask );
        void RemoveBanMask( std::string mask );
        void AllowUserMsg( int client_sd );
        void DeclineUserMsg( int client_sd );
        bool GetIfFull( void );
        

        std::string GetChannelFlags( void );
        //channel flags getters
        bool    GetP_Flag( void );
        bool    GetS_Flag( void );
        bool    GetI_Flag( void );
        bool    GetT_Flag( void );
        bool    GetN_Flag( void );
        bool    GetM_Flag( void );
        int     GetL_Flag( void );

        //channel flags setters
        void    SetP_Flag( bool P );
        void    SetS_Flag( bool S );
        void    SetI_Flag( bool I );
        void    SetT_Flag( bool T );
        void    SetN_Flag( bool N );
        void    SetM_Flag( bool M );
        void    SetL_Flag( int max );


    private:
        std::string         channel_concept;
        std::vector<int>    client_sd_list;
        std::vector<int>    oprtor_sd_list;
        std::vector<std::string>    ban_mask_list;
        std::vector<int>    moderated_channel_allowed; //allowed client_sd in a moderated channel to send msgs to channel

        std::string         channel_key;

        //channel flags 
        bool                private_channel;
        bool                secret_channel;
        bool                invite_only;
        bool                topic_opr_only;
        bool                out_msg_reject;
        bool                moderated_channel;
        int                 channel_max_users;
};

#endif
