#include "../../Log/include/Log.hxx"

#include <pqxx/transaction>
#include <pqxx/pqxx>

#include <iostream>

class Connector {
    public:
        Connector(const std::string& u, 
                const std::string& h,
                const std::string& pswd,
                const std::string& db)
            :user{"user=" + u}, host{" host=" + h}, password{" password=" + pswd}, dbname{" dbname=" + db}
        {
            initConnection(conn, user, host, password, dbname);
            prepareStatements(conn);
        }

        // TODO 
        ~Connector()
        {}

        void initConnection(std::unique_ptr<pqxx::connection>& c, const std::string& u, 
                const std::string& h,
                const std::string& pswd,
                const std::string& db)
        {
            try {
                conn = std::make_unique<pqxx::connection>(user + host + password + dbname);
                std::cout << "Connected to " << conn->dbname() << '\n';
            }
            catch (const std::exception& e) {
                Log() << e.what();
                std::cerr << e.what() << '\n';
                exit(1);
            }
        }

        std::unique_ptr<pqxx::result>& performQuery(const std::string& query)
        {
            trans = std::make_unique<pqxx::work>(*conn, "trans"); 
            res = std::make_unique<pqxx::result>(trans->exec(query));
            trans->commit();

            return res;
        }

    private:
        /* Ban default constructor */
        Connector();

        void prepareListStatement(std::unique_ptr<pqxx::connection>& c)
            
        {
            /*
            - List 
            'SELECT '+selection+' FROM download_queue WHERE '+filter+' '+userClause(options)+' ORDER BY '+options.order+' '+options.direction
            */
            c->prepare("list", "SELECT $1 FROM download_queue WHERE '+filter+' '+userClause(options)+' ORDER BY '+options.order+' '+options.direction");
        }
        void prepareCleanStatement(std::unique_ptr<pqxx::connection>& c);
        /*
        // Clean 
        'DELETE FROM download_queue WHERE status = 5 '+userClause(options)
        */

        void preparePauseStatement(std::unique_ptr<pqxx::connection>& c);
        /*
        // Pause
        'UPDATE download_queue SET status = 3 WHERE (status = 1 OR status = 2) '+userClause(options)+" "+idClause(ids)
        */
        void prepareResumeStatement(std::unique_ptr<pqxx::connection>& c);
        /*
        // Resume 
        'UPDATE download_queue SET status = 1 WHERE (status = 3) '+userClause(options)+" "+idClause(ids)
        */

        void prepareRestartStatement(std::unique_ptr<pqxx::connection>& c);
        /*
        // Restart 
        'UPDATE download_queue SET status = 1 WHERE (status = 101 OR status = 107) '+userClause(options)+" "+idClause(ids)
        */
        void prepareRemoveStatement(std::unique_ptr<pqxx::connection>& c);
        /*
        // Pause
        'UPDATE download_queue SET status = 3 WHERE (status = 1 OR status = 2) '+userClause(options)+" "+idClause(ids)
        */
        void prepareAddWithDestStatement(std::unique_ptr<pqxx::connection>& c);
        /*
        // Add 
        // With given destination 
        "INSERT INTO download_queue (username, url, status, filename, pid, created_time, destination) VALUES (%s, %s, 1, %s, -1, %s, %s)"
        */
        void prepareAddDefaultDestStatement(std::unique_ptr<pqxx::connection>& c);
        /*
        // Add
        // With default destination 
        "INSERT INTO download_queue (username, url, status, filename, pid, created_time) VALUES (%s, %s, 1, %s, -1, %s)"
        */
        void prepareAddTWithDestStatement(std::unique_ptr<pqxx::connection>& c);
        /*
        // Add torrent
        // With given destination
        "INSERT INTO download_queue (username, url, status, filename, pid, created_time, torrent, task_flags, seeding_interval, destination) VALUES (%s, %s, 1, %s, -1, %s, %b, 4, -1, %s)"
        */
        void prepareAddTWithDefaultDestStatement(std::unique_ptr<pqxx::connection>& c);
        /*
        // Add torrent
        // With default destination
        "INSERT INTO download_queue (username, url, status, filename, pid, created_time, torrent, task_flags, seeding_interval) VALUES (%s, %s, 1, %s, -1, %s, %b, 4, -1)"
        */

        /* Execute all statement prep functions */
        void prepareStatements(std::unique_ptr<pqxx::connection>& c)
        {
            prepareListStatement(c);
            prepareCleanStatement(c);
            preparePauseStatement(c);
            prepareResumeStatement(c);
            prepareRestartStatement(c);
            prepareRemoveStatement(c);
            prepareAddWithDestStatement(c);
            prepareAddDefaultDestStatement(c);
            prepareAddTWithDestStatement(c);
            prepareAddTWithDefaultDestStatement(c);
        }

        //void disconnect_connector();
        std::unique_ptr<pqxx::connection> conn;
        std::unique_ptr<pqxx::work> trans;
        std::unique_ptr<pqxx::result> res;

        std::string user;
        std::string host;
        std::string password;
        std::string dbname;
};
