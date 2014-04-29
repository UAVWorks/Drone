﻿CREATE TABLE command_stations (
	id 		smallserial 	PRIMARY KEY,
	ip		inet					NOT NULL,
	port	integer				CHECK (port>0) CHECK (port<65536),
	date	timestamp			with time zone NOT NULL,
	info	varchar(1024)	
);

CREATE TABLE drones (
	id		smallserial	PRIMARY KEY,
	ip		inet				NOT NULL,
	port	integer			CHECK (port>0) CHECK (port<65536),
	date	timestamp		with time zone NOT NULL,
	info	varchar(1024)	
);

CREATE SEQUENCE sessions_id_seq 
	INCREMENT BY 1
	MINVALUE 1
	MAXVALUE 15
	START WITH 1
	NO CYCLE;

CREATE TABLE sessions (
	id				smallint	DEFAULT nextval('sessions_id_seq') PRIMARY KEY,
	iddrone		integer 	REFERENCES drones (id) NOT NULL,
	idcommand	integer 	REFERENCES command_stations (id) NOT NULL,
	date			timestamp	with time zone NOT NULL
);

CREATE TABLE sessions_central_drones (
	id			smallint 		DEFAULT nextval('sessions_id_seq') PRIMARY KEY,
	iddrone	integer			REFERENCES drones (id) NOT NULL,
	date		timestamp with time zone NOT NULL
);

CREATE TABLE sessions_central_commands (
	id				smallint 		DEFAULT nextval('sessions_id_seq') PRIMARY KEY,
	idcommand	integer			REFERENCES command_stations (id) NOT NULL,
	date			timestamp with time zone NOT NULL
);
