set(CLIENT_SRC client/client.c)
add_executable(client ${CLIENT_SRC})
target_link_libraries(client LibreSSL::TLS)

set(SERVER_SRC server/server.c)
add_executable(server ${SERVER_SRC})
target_link_libraries(server LibreSSL::TLS)

set(PROXY_SRC proxy/proxy.c)
add_executable(proxy ${PROXY_SRC})
target_link_libraries(proxy LibreSSL::TLS)
