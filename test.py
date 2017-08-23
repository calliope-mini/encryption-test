from Crypto.Cipher import AES

cipher = AES.new("41eac07039b29abc41eac07039b29abc".decode("hex"), AES.MODE_ECB, "" )
data = "4489959C13C3BC81CA450353EBA08819".decode("hex")
r = b''
for i in [0, 16]:
    r += cipher.decrypt(data[i:i+16])
print r
