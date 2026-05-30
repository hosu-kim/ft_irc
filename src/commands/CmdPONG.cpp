/**
 * Command: PONG
 * Description: Sent in response to a PING to indicate that the connection is still alive.
 *              Clients reply with PONG when they receive a server PING, and the server
 *              may also send PONG replies in certain cases. No further automatic reply
 *              is required upon receiving a PONG.
 *
 * Syntax: PONG <server/client_name> [:message]
 *
 * Examples:
 *    Case 1. Client responding to server PING
 *            - Server sends: PING :srv123
 *            - Client sends back: PONG :srv123
 *
 *    Case 2. Server responding to client-initiated PING
 *            - Client sends: PING :hello
 *            - Server sends back: :srv123 PONG srv123 :hello
 */

#include "CmdPONG.hpp"

/*
1. // Validate parameters
   if (getParamCount() == 0) {
       // No token/message provided — nothing to do.
       // Optionally log a warning or ignore silently.
       return;
   }

2. // Parse target token and optional trailing message
   token = _params[0]
   if token starts with ':' then token = token.substr(1)

   message = ""
   if getParamCount() > 1:
       // join remaining params with spaces into message
       message = join(_params[1..], " ")
       if message starts with ':' then message = message.substr(1)

3. // Update keep-alive / ping state
   // Record that this user responded to a ping
   user.setLastPongTime(now())      // or store a timestamp field

   // If server was waiting for a PONG from this user:
   if server.hasPendingPingFor(user):
       expectedToken = server.getPendingPingToken(user)
       if expectedToken == token:
           // Optionally compute RTT if server stored ping timestamp
           sentAt = server.getPendingPingTimestamp(user)
           rtt = now() - sentAt
           user.setPingLatency(rtt)    // optional
           server.clearPendingPingFor(user)

4. // Logging / monitoring (optional)
   server.log("PONG received from " + user.getUserName() + " token=" + token + " msg=" + message)

5. // No reply should be sent in response to a PONG
   return
*/