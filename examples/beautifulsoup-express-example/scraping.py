
import urllib.request
import certifi
from bs4 import BeautifulSoup

def links(url):
    try:
        # Open URL for reading the HTML (allowing SSL certificates with certifi)
        fp = urllib.request.urlopen(url, cafile=certifi.where())

        # Read data as a bytearray
        data = fp.read()

        # Convert into a utf8 string
        string = data.decode("utf8")

        # Close URL
        fp.close()

        # Initialize beautiful soup in order to scrape the web
        soup = BeautifulSoup(string, features='html.parser')

        # Obtain all href for each link
        links = map(lambda a: a.get('href'), soup.find_all('a'))

        # Filter the ones that begin with http
        result = filter(lambda link: link.startswith('http'), links)

        # Return it as a list
        return list(result)
    except:
        return []
