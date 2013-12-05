#!/usr/bin/python -tt
# This program is free software; you can redistribute it and.or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Library General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
#
#
# Examples:
#
#  yum install --stage test package
#  yum install --stage stable package

import yum
import yum.plugins
import rpmUtils.arch

import locale
import os
import os.path

locale.setlocale(locale.LC_ALL, '') 

requires_api_version = '2.1'
if yum.plugins.API_VERSION < '2.3':
    from yum.plugins import TYPE_INTERFACE
    plugin_type = (TYPE_INTERFACE,)
else:
    from yum.plugins import TYPE_INTERACTIVE
    plugin_type = (TYPE_INTERACTIVE,)

def config_hook(conduit):
    parser = conduit.getOptParser()
    parser.add_option('', '--stage', '-s', '--st', action='store',
           type='string', dest='stage', default='stable',
           metavar='[stable|test]',
           help="specifies the stage of rpm on yum server(default stable)")

#added by duanjigang@2013-09-21 --start
def clean_hook(conduit):
  path="/var/cache/yum"
  for root,dirs,files in os.walk(path):
    for filespath in files:
      if "develop" in os.path.join(root,filespath):
          os.remove(os.path.join(root,filespath))
#added by duanjigang@2013-09-21 --finish

def prereposetup_hook(conduit):
    opts, commands = conduit.getCmdLine()
    repos = conduit.getRepos()
    # Display the options from the [main] section
    basearch = rpmUtils.arch.getBaseArch()
    releasemaj, releaseminor = getRhelRelease()
    releasemaj = releasemaj + "Server" 
    if opts.stage == 'test':
        repos.enableRepo('develop.' + releasemaj + '.' + basearch + '.test')
        repos.enableRepo('develop.' + releasemaj + '.' + basearch + '.stable')
        repos.enableRepo('develop.' + releasemaj + '.noarch.test')
        repos.enableRepo('develop.' + releasemaj + '.noarch.stable')
    elif opts.stage == 'stable':
        repos.enableRepo('develop.' + releasemaj + '.' + basearch + '.stable')
        repos.enableRepo('develop.' + releasemaj + '.noarch.stable')
    else:
        print "WARNING:unknown stage " + opts.stage + ", ignore it."
        repos.enableRepo('develop.' + releasemaj + '.' + basearch + '.stable')
        repos.enableRepo('develop.' + releasemaj + '.noarch.stable')
    # Display the options from the repository sections

def getRhelRelease():
    """
    get RHEL Release version from /etc/redhat-release
    """
    rf = open('/etc/redhat-release', 'r')
    if rf:
        r = rf.readline().split(' ')
        if r[6] == '4':
            maj = '4AS'
            minor = r[9][0]
            return maj, minor
        elif r[6][0] == '5Sever':
            return r[6].split('.')
        elif r[6][0] == '6Server':
            return r[6].split('.')
        else:
            return False


