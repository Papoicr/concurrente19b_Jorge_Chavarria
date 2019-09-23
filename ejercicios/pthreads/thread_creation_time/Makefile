APPNAME=thread_creation_time
$(APPNAME): $(APPNAME).c
	cc -g -Wall -Wextra $(APPNAME).c -o $(APPNAME) -pthread

.PHONY: clean
clean:
	rm -f $(APPNAME)
