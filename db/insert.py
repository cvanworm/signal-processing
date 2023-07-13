import sqlite3
import sys

def insertWorkerInfo(host,cores,mem_avail,uptime,mem_used,load):
    conn = sqlite3.connect('worker.db')
    cursor = conn.cursor()
    insert = """INSERT INTO WORKER_DATA (HOSTID,CORES,MEMAVAIL,UPTIME,MEMUSED,LOADAVG)
                VALUES (?, ?, ?, ?, ?, ?);"""
    
    data = (host,cores,mem_avail,uptime,mem_used,load)

    cursor.execute(insert, data)
    conn.commit()

    cursor.close()
    conn.close()

#insertWorkerInfo("test",5,1000,20,1000,0)
insertWorkerInfo(sys.argv[1],sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5], sys.argv[6])