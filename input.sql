create table
    Todos (
        Id integer PRIMARY KEY NOT NULL,
        Title varchar(128) NOT NULL,
        Completed integer NOT NULL
    );

insert into
    Todos
values
    (NULL, 'code this app', 0);

insert into
    Todos
values
    (NULL, 'cook dinner', 0);