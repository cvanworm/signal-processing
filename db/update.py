import sqlite3
import sys

def updateWorkerInfo(host,mem_avail,uptime,mem_used,load):
    conn = sqlite3.connect('worker.db')
    cursor = conn.cursor()
    update = """UPDATE WORKER_DATA
                SET MEMAVAIL = ?,
                    UPTIME = ?,
                    MEMUSED = ?,
                    LOADAVG =?
                WHERE HOSTNAME = ?"""
    
    data = (mem_avail,uptime,mem_used,load,host)

    cursor.execute(update, data)
    conn.commit()

    cursor.close()
    conn.close()

#updateWorkerInfo("test",0,10,2000,3)
updateWorkerInfo(sys.argv[1],sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5])