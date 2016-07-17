cc_library(
  name='rsa',
  srcs=[
    'rsa.cc',
    'rsa_key.cc'
  ],
  deps=[
    '//common/encoding:encoding',
    '//thirdparty/openssl:crypto',
  ]
)

rsa_test_data=[
  'test_data/private_key.pem',
  'test_data/public_key.pem',
  'test_data/PUB_key.pem',
]

cc_test(
  name='rsa_key_test',
  srcs=['rsa_key_test.cc'],
  deps=[':rsa'],
  testdata=rsa_test_data
)

cc_test(
  name='rsa_test',
  srcs=['rsa_test.cc'],
  deps=[':rsa'],
  testdata=rsa_test_data
)

cc_binary(
  name='rsa_demo',
  srcs=['rsa_demo.cc'],
  deps=[
    ':rsa',
    '//thirdparty/gflags:gflags',
    '//thirdparty/glog:glog',
   ]
)

