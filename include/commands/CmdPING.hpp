/* PING-PONG: The server and client use it to check if each other is alive.

   Syntax: PING <server>/<client>

   Example: 1. Client-side
               - Client sends: `PING :hello`
               - Server sends back: `:srv123 PONG srv123 :hello`
            2. Server-side
               - Server sends: `PING :srv123
               - Clien sends back: `PONG :srv123
*/