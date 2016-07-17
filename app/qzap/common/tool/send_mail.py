#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013, Tencent Inc.
# All rights reserved.
#
# Author: Chen Feng <phongchen@tencent.com>
#
# Send mail from GDT_CODE@tencent.com

import smtplib
import string
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText


_HOST = 'tsmtp.tencent.com'


def to_tencent_email(mail):
    if not mail:
        return mail
    if isinstance(mail, basestring):
        if '@' in mail:
            mail_address = mail
        else:
            mail_address = mail + '@tencent.com'
        return [mail_address]
    result = []
    for m in mail:
        result.extend(to_tencent_email(m))
    return result


def send_mail_html(user_name, password, to, cc=[], reply_to='', subject='',
                   text='', html=''):
    to = to_tencent_email(to)
    cc = to_tencent_email(cc)
    reply_to = to_tencent_email(reply_to)
    # Create message container - the correct MIME type is multipart/alternative.
    msg = MIMEMultipart('alternative')
    msg['Subject'] = subject
    msg['From'] = user_name
    msg['To'] = ','.join(to)
    if cc:
        msg['CC'] = ','.join(cc)
    if reply_to:
        msg.add_header('reply-to', ','.join(reply_to))

    # Record the MIME types of both parts - text/plain and text/html.


    # Attach parts into message container.
    # According to RFC 2046, the last part of a multipart message, in this case
    # the HTML message, is best and preferred.
    if text:
        msg.attach(MIMEText(text.encode('utf-8'), 'plain'))
    if html:
        msg.attach(MIMEText(html.encode('utf-8'), 'html'))

    # Send the message via local SMTP server.
    s = smtplib.SMTP(_HOST)
    s.login(user_name, password)
    # sendmail function takes 3 arguments: sender's address, recipient's address
    # and message to send - here it is sent as one string.
    s.sendmail(user_name, set(to + cc), msg.as_string())
    s.quit()
