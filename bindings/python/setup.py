from setuptools import setup, find_packages

setup(
	name="pssc-taskdev",
	description="Python bindings for taskdev library",
	author_email="ksyusha.churkina.2005@mail.ru",
	packages=find_packages(),
	install_requires=["cffi"]
)	
