import imaplib
import email

def extract_body(payload):
    if isinstance(payload,str):
        return payload
    else:
        return '\n'.join([extract_body(part.get_payload()) for part in payload])

conn = imaplib.IMAP4_SSL("imap.gmail.com", 993)
conn.login("sprudhvichandra@gmail.com", "Qwerty12#")
conn.select()
typ, data = conn.search(None, '(SUBJECT "The contents of measurements.txt")')
try:
    for num in data[0].split():
        typ, msg_data = conn.fetch(num, '(RFC822)')
        for response_part in msg_data:
            if isinstance(response_part, tuple):
                msg = email.message_from_string(response_part[1])
                subject=msg['subject']                   
                #print(subject)
                payload=msg.get_payload()
                body=extract_body(payload)
                date = msg["Date"]
                #print(body)
                #print date
                text = '"' + date + '"' + '@' + body
        typ, response = conn.store(num, '+FLAGS', r'(\Seen)')
    fo = open("measurements_read.txt", "w")
    fo.write(text)
    fo.close()
    #print text
finally:
    try:
        conn.close()
    except:
        pass
    conn.logout()