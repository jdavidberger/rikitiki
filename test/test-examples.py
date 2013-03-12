#/usr/bin/python
import urllib.request
from time import sleep
import traceback
import subprocess
import http.client

test_points = {
    "helloWorld": [ "/hw/hello", 
                    "/hw/test",
                    "/hw/131"
                    ],
    "advanced": [ "/adv",
                  "/adv/post",
                  "/adv/qs",
                  "/adv/qs?test=qs"
                  ],
    "headersTest": [
        "/headers",
        "/cookies",
        "/cookies?name=asdf&value=1234"
        ],
    "rest": [
        ("DELETE", "/book"),
        ("GET", "/book"),
        ("POST", "/book", '{"name":"lotr","author":"tolkien","isbn":"123-456"}'),
        ("GET", "/book"),
        ("DELETE", "/book"),
        ("GET", "/book"),
        ("POST", "/book", '{"name":"lotr","author":"tolkien","isbn":"123-456}'),
        ("POST", "/book", '{"name":"lotr","author":"tolkien","isbn":"123-4568"}'),
        ("POST", "/book", '{"name":"lotr","author":"tolkien","isbn":"123-4567"}'),
        ("GET", "/book"),
        ("GET", "/book/1"),
        ("GET", "/book/2"),
        ("GET", "/book/3")
        ],
    "restAdv": [
        ("DELETE", "/book-adv"),
        ("GET", "/book-adv"),
        ("POST", "/book-adv", '{"name":"lotr","author":"tolkien","isbn":"123-456"}'),
        ("GET", "/book-adv"),
        ("DELETE", "/book-adv"),
        ("GET", "/book-adv"),
        ("POST", "/book-adv", '{"name":"lotr","author":"tolkien","isbn":"123-456}'),
        ("POST", "/book-adv", '{"name":"lotr","author":"tolkien","isbn":"123-4568"}'),
        ("POST", "/book-adv", '{"name":"lotr","author":"tolkien","isbn":"123-4567"}'),
        ("GET", "/book-adv"),
        ("GET", "/book-adv/1"),
        ("GET", "/book-adv/2"),
        ("GET", "/book-adv/3")
        ],
    "multiType": [
        "/mt/test/1.23/1231.2123/10/a/testing/matchthistoo",
        "/mt/test/1.23/1231.2123/10/a/testing/matchthis",
        "/mt/test/0.000000000000000000000000001/-1231.2123/-10/_/testing/matchthistoo",
        ]

}

def make_dir(d):
    import os
    try:
        os.makedirs(d)
    except OSError:
        pass

def resp_to_string(response):
    rtn = ""
    rtn += str(response.status) + " " + str(response.reason) + "\r\n"
    for header in response.getheaders():
        if(not (header[0] == "Date") and not (header[0] == "Server")):
            rtn += header[0] + ": " + header[1] + "\r\n"
    rtn += "\r\n"
    rtn += response.read().decode('utf8')
    rtn += "\r\n"
    return rtn

def compare_files(fileA, fileB):
    fileA_file = open(fileA, 'r')
    fileB_file = open(fileB, 'r')
    a = fileA_file.read()
    b = fileB_file.read()
    return a == b

def create_response( conn, tp ):
    method = "GET"
    data = ""
    url = ""
    headers = {'content-type': 'application/json'}
    if isinstance(tp, tuple):
        method = tp[0]
        url = tp[1]
        if len(tp) == 3:
            data = tp[2]        
    else:
        url = tp
    conn.request(method, url, body=data, headers=headers)

    return conn.getresponse()

def test_server(server, port):
    root = "localhost:" + str(port)
    make_dir("output/" + server)
    make_dir("verified/" + server)
    print("Testing " + server)
    for module in test_points:
        print("\tTesting " + module)
        for i in range(len(test_points[module])):
            conn = http.client.HTTPConnection(root)
            tp = test_points[module][i]            
            try:
                response = create_response(conn, tp)
                test_data = resp_to_string( response )
                output_file = 'output/' + server + '/' + module + "." + str(i)
                verify_file = 'verified/' + server + '/' + module + "." + str(i)
                f = open(output_file, 'w')
                f.write(test_data)
                f.close()
                if compare_files(verify_file, output_file):
                    print("\t\t\033[92mPassed\033[0m " + str(tp))
                else:
                    print("\t\t\033[91mFailed\033[0m " + str(tp))
            except Exception as e:
                print("\t\t\033[91mFailed\033[0m " + str(tp) + " with exception " + str(type(e)))
                ferr = open( 'output/' + server + '/' + module + "." + str(i) + ".err",'w')                
                traceback.print_exc(file=ferr)
                ferr.close()
            conn.close()

test_server('apache', 80)
test_server('mongoose', 5000)    
