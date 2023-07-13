import sqlite3

conn = sqlite3.connect('worker.db')
cursor = conn.cursor()

#Doping EMPLOYEE table if already exists.
cursor.execute("DROP TABLE IF EXISTS WORKER_DATA")
conn.execute('''CREATE TABLE WORKER_DATA
         (HOSTID         TEXT     NOT NULL,
         CORES             TEXT     NOT NULL,
         MEMAVAIL          TEXT     NOT NULL,
         UPTIME            TEXT     NOT NULL,
         MEMUSED           TEXT     NOT NULL,
         LOADAVG           TEXT   NOT NULL);''')
print ("Table created successfully")

conn.close()