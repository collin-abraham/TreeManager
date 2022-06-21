2022-06-09

This program simulates a business that uses one simple table in a postgres database that requires full CRUD within a console application. It uses libpqxx as an API to talk to the PostgreSQL database. 

If you are interested in running this locally, you need to have at least version 14.3 of postgres installed. I would suggest using PgAdmin4 to administer the database. 

1) Create a PostgreSQL database 
2) Run the Script in sqlscript.sql contained in the project to create the table and insert a few values for testing
3) Ensure your Visual Studio is set to Release mode and x64 (sometimes running in debug/x86 causes issues with libpqxx)
4) Edit the connection string function connString() in buildFunctions.cpp to match your local database details 
