/*
    PURPOSE: (Abstract base class for external applications.)
*/

#ifndef EXTERNAL_APPLICATION_HH
#define EXTERNAL_APPLICATION_HH

#include <string>
#include <sstream>

namespace Trick {

    /**
     * Abstract base class for external applications to be started automatically.
     *
     * @see @ref ExternalApplications "Trick GUIs"
     *
     * @author Derek Bankieris
     */
    class ExternalApplication {

        public:

        /** Behavior to take on disconnect. */
        enum DisconnectBehavior {CLOSE, NOTIFY, NOTHING};

        /** Constructor. */
        ExternalApplication();

        /** Destructor. */
        virtual ~ExternalApplication() {}

        /**
         * Sets #command.
         *
         * @param command the launch command
         */
        virtual void set_startup_command(std::string command);

        /**
         * Gets #command.
         *
         * @return the launch command
         */
        virtual std::string get_startup_command();

        /**
         * Appends #arguments.
         *
         * @param args the arguments to append
         */
        virtual void add_arguments(std::string args);

        /**
         * Sets #arguments.
         *
         * @param args the arguments
         */
        virtual void set_arguments(std::string args);

        /**
         * Gets #arguments.
         *
         * @return the current arguments
         */
        virtual std::string get_arguments();

        /**
         * Sets #host.
         *
         * @param host the Variable Server host name
         */
        virtual void set_host(std::string host);

        /**
         * Gets #host.
         *
         * @return the Variable Server host name
         */
        virtual std::string get_host();

        /**
         * Sets #port.
         *
         * @param port the Variable Server port
         */
        virtual void set_port(unsigned short port);

        /**
         * Gets #port.
         *
         * @return the Variable Server port
         */
        virtual unsigned short get_port();

        /**
         * Sets #auto_reconnect.
         *
         * @param auto_reconnect the launch reconnection behavior
         */
        virtual void set_auto_reconnect(bool auto_reconnect);

        /**
         * Gets #auto_reconnect.
         *
         * @return the launch reconnection behavior
         */
        virtual bool get_auto_reconnect();

        /**
         * Sets #cycle_period.
         *
         * @param period the cycle period
         */
        virtual void set_cycle_period(double cycle_period);

        /**
         * Gets #cycle_period.
         *
         * @return the cycle period
         */
        virtual double get_cycle_period();

        /**
         * Sets #minimum_cycle_period.
         *
         * @param period the minimum_cycle period
         */
        virtual void set_minimum_cycle_period(double minimum_cycle_period);

        /**
         * Gets #minimum_cycle_period.
         *
         * @return the minimum_cycle period
         */
        virtual double get_minimum_cycle_period();

        /**
         * Sets #disconnect_behavior.
         *
         * @param disconnect_behavior the behavior to take on disconnect
         */
        virtual void set_disconnect_behavior(DisconnectBehavior disconnect_behavior);

        /**
         * Gets #disconnect_behvaior.
         *
         * @return the behavior to take on disconnect
         */
        virtual DisconnectBehavior get_disconnect_behavior();

        /**
         * Sets #height.
         *
         * @param height the window's initial height
         */
        virtual void set_height(int height);

        /**
         * Gets #height.
         *
         * @return the window's initial height
         */
        virtual int get_height();

        /**
         * Sets #width.
         *
         * @param width the window's initial width
         */
        virtual void set_width(int width);

        /**
         * Gets #width.
         *
         * @return the window's initial width
         */
        virtual int get_width();

        /**
         * Sets #x.
         *
         * @param x the window's initial x location
         */
        virtual void set_x(int x);

        /**
         * Gets #x.
         *
         * @return the window's initial x location
         */
        virtual int get_x();

        /**
         * Sets #y.
         *
         * @param y the window's initial y location
         */
        virtual void set_y(int y);

        /**
         * Gets #y.
         *
         * @return the window's initial y location
         */
        virtual int get_y();

        /**
         * Launches this application if the Variable Server is enabled.
         * This is automatically called by %Trick during simulation initialization.
         */
        virtual void launch();

        protected:

        /** Designates from which source an option is to be used. */
        enum OptionSource {AUTO, USER, NONE};

        /** Command to execute when starting this application. */
        std::string command;

        /** Arguments to be passed after the command. */
        std::ostringstream arguments;

        /** Variable Server host. */
        std::string host;

        /** The source from which to obtain the Variable Server host. */
        OptionSource host_source; /* ** ignore */

        /** Variable Server port. */
        unsigned short port;

        /** The source from which to obtain the Variable Server port. */
        OptionSource port_source; /* ** ignore */

        /** Whether or not to reestablish the previous connection on launch. */
        bool auto_reconnect;

        /** Indicates that the user has specified a reconnection launch behavior. */
        bool auto_reconnect_set;

        /** Period at which to receive updates. */
        double cycle_period;

        /** Indicates that the user has specified a cycle period. */
        bool cycle_period_set;

        /** Period at which to receive updates. */
        double minimum_cycle_period;

        /** Indicates that the user has specified a minimum_cycle period. */
        bool minimum_cycle_period_set;

        /** Behavior to take on disconnect. */
        DisconnectBehavior disconnect_behavior;

        /** Indicates that the user has specified a disconnect behavior. */
        bool disconnect_behavior_set;

        /** Height of the window at launch. */
        int height;

        /** Indicates the the user has specified a height. */
        bool height_set;

        /** Width of the window at launch. */
        int width;

        /** Indicates that the user has specified a width. */
        bool width_set;

        /** X-coordinate of the window at launch. */
        int x;

        /** Indicates that the user has specified an x-coordinate. */
        bool x_set;

        /** Y-coordinate of the window at launch. */
        int y;

        /** Indicates that the use rhas specified a y-coordinate. */
        bool y_set;

        /** Creates a string containing the arguements that this application explicitly supports. */
        virtual std::string create_arguments_string();

        private:

        /** Prevent SWIG from trying to invoke operator= on ostringstream. */
        ExternalApplication &operator=(const ExternalApplication &);

    };

}
#endif
