install.packages("Rfacebook")
library(devtools)

install_github("Rfacebook", "pablobarbera", subdir="Rfacebook")
library (Rfacebook)



fb_oauth <- fbOAuth(app_id="697089080490983", app_secret="5340a7132a84bee2f9da60541b8a005b",extended_permissions = TRUE)

#save(fb_oauth, file="~/CourseWork/2017Sem02CourseWork/ICS5115/fb_oauth")
#load("fb_oauth")


me <- getUsers("me",token="EAACEdEose0cBAHGPZAwbdbYiWeqnXQzn7s5ZBMZCIgcWtzlZAyLKY5w1bRT8nHGJHIEmz7ZAqA1j7g1Iv9qXTVyZBP1x7u3vKufnCGVU3k2HZAQZAZB7i1XDS5kCDUFohRseMeTWvZCKZBCglvONfMXNMLr0JSFh9vYjuaq4SfCm8tldBx8A2ZCzOl6qvJBQHxwMnUgZD")
my_likes <- getLikes(user="me", token=fb_oauth)

my_friends <- getFriends(token="EAACEdEose0cBAHGPZAwbdbYiWeqnXQzn7s5ZBMZCIgcWtzlZAyLKY5w1bRT8nHGJHIEmz7ZAqA1j7g1Iv9qXTVyZBP1x7u3vKufnCGVU3k2HZAQZAZB7i1XDS5kCDUFohRseMeTWvZCKZBCglvONfMXNMLr0JSFh9vYjuaq4SfCm8tldBx8A2ZCzOl6qvJBQHxwMnUgZD", simplify = TRUE)

token = "EAACEdEose0cBAHGPZAwbdbYiWeqnXQzn7s5ZBMZCIgcWtzlZAyLKY5w1bRT8nHGJHIEmz7ZAqA1j7g1Iv9qXTVyZBP1x7u3vKufnCGVU3k2HZAQZAZB7i1XDS5kCDUFohRseMeTWvZCKZBCglvONfMXNMLr0JSFh9vYjuaq4SfCm8tldBx8A2ZCzOl6qvJBQHxwMnUgZD"

my_friends_info <- getUsers(my_friends$id, token, private_info = TRUE)
mat <- getNetwork(token, format = "adj.matrix")
getUsers("me", token=fb_oauth, private_info = FALSE)