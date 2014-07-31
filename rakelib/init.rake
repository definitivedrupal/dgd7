desc "Create a central bare repository from the local repository."
task :init_repo do
  repo_id = File.basename(Dir.getwd)
  repo_path = "/srv/git/#{repo_id}.git"
  post_receive = "/usr/bin/kgb-client --git-reflog - --conf /etc/kgb-client/kgb.conf --repository git --repo-id #{repo_id}"

  commands = [
    "mkdir #{repo_path}",
    "cd #{repo_path}",
    "git init --bare",
    # For HTTP access such as gitweb
    "mv hooks/post-update.sample hooks/post-update",
    # For KGB to send commit notifications over IRC.
    # This still requires additional server-side configuration, see
    # http://my.agaric.com/agaric/node/11694
    "echo #{post_receive} >> hooks/post-receive",
    "chmod a+x hooks/post-receive"
  ].join(" && ")
  sh "ssh git.agaric.com '#{commands}'"
  sh "git remote add origin git.agaric.com:#{repo_path}"
  sh "git push origin master"
end

desc "Add .gitignore file with basic project specific patterns."
task ".gitignore" do
  open(".gitignore", "a") do |f|
    f.puts ".sass-cache/"
    f.puts ".vagrant/"
    f.puts "build/"
    f.puts "web/sites/default/settings.php"
    f.puts "web/sites/default/files/"
  end
end

desc "Add .gitattributes file with basic project specific patterns."
task ".gitattributes" do
  open(".gitattributes", "a") do |f|
    f.puts "etc export-ignore"
  end
end
